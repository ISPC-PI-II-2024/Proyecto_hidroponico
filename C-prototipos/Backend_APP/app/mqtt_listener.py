import json
import threading
import paho.mqtt.client as mqtt
from app.config import settings
from app.utils.logger import get_logger
from app.models.mensaje import GatewayMessage, DeviceInfo
from app.services.mariadb_serv import procesar_info_inicial, guardar_datos

logger = get_logger("mqtt_listener")

#========================================
# Configuración de la aplicación (CLASE)
#========================================

class MQTTListener:
    """
    Se encarga de:
      - Conectar al broker MQTT
      - Suscribirse a los topics indicados en settings.mqtt_topics
      - Procesar cada mensaje y derivarlo al servicio adecuado. En esta caso a la base en MariaDB
    """
    #------------------------------------------
    # CREACION DEL CLIENTE MQTT
    #------------------------------------------
    def __init__(self):
        # Creamos el cliente MQTT
        self.client = mqtt.Client()
        #if settings.mqtt_user and settings.mqtt_password:
        self.client.username_pw_set(settings.mqtt_user, settings.mqtt_password)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message

    #------------------------------------------
    # ARRAQUE DE CONEXION CON EL BROKER
    #------------------------------------------
    def start(self):
        logger.info(f"Conectando a broker MQTT en {settings.mqtt_broker_url}...")

        url = settings.mqtt_broker_url.replace("mqtt://", "")
        host, port = url.split(":")
        self.client.connect(host, int(port))

        # Se levanta el loop en background para no bloquear la app HTTP
        threading.Thread(target=self.client.loop_forever, daemon=True).start()
        logger.info("MQTT listener arrancado en background.")

    #------------------------------------------
    # ACCIONES AL CONECTAR (SUBSCRIPCION A TOPICS)
    #------------------------------------------
    def _on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            logger.info("Conexión MQTT exitosa.")
                # Nos suscribimos a cada topic, en caso de haber mas de uno
            for topic in settings.mqtt_topic_info + settings.mqtt_topic_data:
                client.subscribe(topic)
                logger.info(f"Suscrito al topic '{topic}'.")
        else:
            logger.error(f"Fallo al conectar MQTT, código de error: {rc}")

    #------------------------------------------
    # ACCIONES AL RECIBIR MENSAJE
    #------------------------------------------

    def _on_message(self, client, userdata, msg):
        try:
            data = json.loads(msg.payload.decode("utf-8"))
            logger.info(f"Mensaje en '{msg.topic}': {data}")
        except Exception as e:
            logger.error(f"JSON inválido en '{msg.topic}': {e}")
            return

        # → Payload de INFO (configuración inicial)
        if msg.topic in settings.mqtt_topic_info:
            try:
                info = DeviceInfo.parse_obj(data)
                procesar_info_inicial(info)
                logger.info("INFO inicial guardada en MariaDB.")
            except Exception as e:
                logger.error(f"Error guardando INFO: {e}")

        # → Payload de DATOS (lecturas y controles)
        elif msg.topic in settings.mqtt_topic_data:
            try:
                lectura = GatewayMessage.parse_obj(data)
                guardar_datos(lectura)
                logger.info("Datos de sensores y controles guardados en MariaDB.")
            except Exception as e:
                logger.error(f"Error guardando DATOS: {e}")

        else:
            logger.warning(f"Tópico '{msg.topic}' no mapeado.")


#------------------------------------------
# INTREGRACION EN MAIN.PY
#------------------------------------------
listener = MQTTListener()
