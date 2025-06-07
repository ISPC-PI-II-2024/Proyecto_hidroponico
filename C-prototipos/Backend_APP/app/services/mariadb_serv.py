from typing import Optional, List
from datetime import datetime
from dateutil import parser
from dateutil.parser import ParserError

from app.utils.logger import get_logger
from app.models.mensaje import GatewayMessage
import sqlalchemy as sa
from sqlalchemy import Table, MetaData, Column, String, DateTime, Boolean, Float, Integer, func
from app.config import settings

logger = get_logger("mariadb_service")

#========================================
# Configuración SQLAlchemy
#========================================
_engine = sa.create_engine(settings.mariadb_url)
_metadata = MetaData()

# Tabla de dispositivos
_device_table = Table(
    "Tabla dispostivo", # Hay que cambiarlo cuando tengamos la info de la tabla definida
    _metadata,
    Column("device_id", String(64), primary_key=True),
    Column("name", String(128), nullable=False),
    Column("created_at", DateTime, nullable=False, server_default=func.current_timestamp()),
    Column("transmitter", Boolean, nullable=False, default=False),
    Column("receiver", Boolean, nullable=False, default=False),
    Column("gateway", Boolean, nullable=False, default=False),
)

# Tabla de datos de sensores
_sensor_table = Table(
    "Tbla datos sensor", # Hay que cambiarlo cuando tengamos la info de la tabla definida
    _metadata,
    Column("id", sa.Integer, primary_key=True, autoincrement=True),
    Column("device_id", String(64), nullable=False),
    Column("timestamp", DateTime, nullable=False),
    Column("temperatura_agua", Float),
    Column("temperatura_aire", Float),
    Column("humedad", Float),
    Column("nivel_agua", Float),
    Column("flujo_agua", Float),
    Column("luz", Integer),
    Column("gas", Integer),
    Column("corriente", Float),
    Column("voltaje", Float),
    Column("potencia", Float),
    Column("ph_valor", Float),
    Column("hora_local", DateTime, nullable=True),
)

# Tabla de controles
_control_table = Table(
    "Tabla Controladores", # Hay que cambiarlo cuando tengamos la info de la tabla definida
    _metadata,
    Column("id", sa.Integer, primary_key=True, autoincrement=True),
    Column("device_id", String(64), nullable=False),
    Column("timestamp", DateTime, nullable=False),
    Column("bomba", Boolean),
    Column("luces", Boolean),
    Column("alarma", Boolean),
    Column("modo_automatico", Boolean),
)

_metadata.create_all(_engine)

#========================================
# Función que registra info en MariaDB
#========================================
def guardarDatos(msg: GatewayMessage) -> None:
    logger.debug(f"Guardando información de dispositivos en MariaDB: {msg}")
    with _engine.connect() as conn:
        for node in msg.nodes:
            gateway_flag = node.deviceId == msg.gatewayId

            # Guardar info de dispositivo
            ins_device = sa.insert(_device_table).values(
                device_id=node.deviceId,
                name=node.deviceId,
                transmitter=node.transmitter or False,
                receiver=node.reciber or False,
                gateway=gateway_flag
            ).prefix_with("IGNORE")
            conn.execute(ins_device)

            # Guardar datos de sensores
            sensor = node.sensors
            ins_sensor = sa.insert(_sensor_table).values(
                device_id=node.deviceId,
                timestamp=node.timestamp,
                temperatura_agua=sensor.temperaturaAgua,
                temperatura_aire=sensor.temperaturaAire,
                humedad=sensor.humedad,
                nivel_agua=sensor.nivelAgua,
                flujo_agua=sensor.flujoAgua,
                luz=sensor.luz,
                gas=sensor.gas,
                corriente=sensor.corriente,
                voltaje=sensor.voltaje,
                potencia=sensor.potencia,
                ph_valor=sensor.phValor,
                hora_local=sensor.hora
            )
            conn.execute(ins_sensor)

            # Guardar datos de controles
            control = node.controls
            ins_control = sa.insert(_control_table).values(
                device_id=node.deviceId,
                timestamp=node.timestamp,
                bomba=control.bomba,
                luces=control.luces,
                alarma=control.alarma,
                modo_automatico=control.modoAutomatico
            )
            conn.execute(ins_control)

        conn.commit()
    logger.info("Información de dispositivos, sensores y controles escrita en MariaDB.")




# ========================================
# NUEVAS FUNCIONES PARA TOPICOS MQTT
# ========================================

def obtener_id_sistema(conn, nombre_dispositivo: str) -> Optional[int]:
    result = conn.execute(sa.text("""
        SELECT id_sistema FROM sistemas WHERE nombre = :nombre
    """), {'nombre': nombre_dispositivo}).fetchone()
    return result[0] if result else None

def procesar_info_inicial(data: dict):
    logger.info(f"[INFO] Procesando configuración inicial para dispositivo '{data.get('device')}'")
    with _engine.connect() as conn:
        nombre = data["device"]
        id_sistema = obtener_id_sistema(conn, nombre)

        if not id_sistema:
            conn.execute(sa.text("""
                INSERT INTO sistemas (nombre, fecha_instalacion) VALUES (:nombre, CURDATE())
            """), {'nombre': nombre})
            id_sistema = conn.execute(sa.text("""
                SELECT id_sistema FROM sistemas WHERE nombre = :nombre
            """), {'nombre': nombre}).scalar()

        # Guardar sensores
        for s in data.get("sensores", []):
            conn.execute(sa.text("""
                INSERT INTO sensores (nombre, tipo, pin_entrada, id_sistema)
                VALUES (:nombre, 'desconocido', :pin, :id_sistema)
                ON DUPLICATE KEY UPDATE pin_entrada = :pin
            """), {
                'nombre': s['nombre'],
                'pin': s['pin'],
                'id_sistema': id_sistema
            })

            if "umbralTempMax" in s:
                conn.execute(sa.text("""
                    INSERT INTO umbrales (tipo_sensor, valor_min, valor_max, id_sistema)
                    VALUES ('temperatura', NULL, :max, :id_sistema)
                """), {'max': s['umbralTempMax'], 'id_sistema': id_sistema})
            if "umbralHumMin" in s:
                conn.execute(sa.text("""
                    INSERT INTO umbrales (tipo_sensor, valor_min, valor_max, id_sistema)
                    VALUES ('humedad', :min, NULL, :id_sistema)
                """), {'min': s['umbralHumMin'], 'id_sistema': id_sistema})
            if "umbralPresionMin" in s:
                conn.execute(sa.text("""
                    INSERT INTO umbrales (tipo_sensor, valor_min, valor_max, id_sistema)
                    VALUES ('presion', :min, NULL, :id_sistema)
                """), {'min': s['umbralPresionMin'], 'id_sistema': id_sistema})

        # Guardar actuadores
        for a in data.get("actuadores", []):
            conn.execute(sa.text("""
                INSERT INTO actuadores (nombre, tipo, estado_actual, pin_salida, id_sistema)
                VALUES (:nombre, 'rele', :estado, :pin, :id_sistema)
                ON DUPLICATE KEY UPDATE estado_actual = :estado, pin_salida = :pin
            """), {
                'nombre': a['nombre'],
                'estado': a['estado'],
                'pin': a['pin'],
                'id_sistema': id_sistema
            })

        conn.commit()

def procesar_lectura(data: dict):
    logger.info(f"[LECTURA] Procesando lectura para '{data.get('device')}'")
    with _engine.connect() as conn:
        id_sistema = obtener_id_sistema(conn, data["device"])
        if not id_sistema:
            logger.error(f"No se encontró el sistema con nombre '{data['device']}'")
            return

        sensores = data.get("sensores", {})
        fecha_hora = datetime.fromtimestamp(data.get("timestamp", datetime.now().timestamp()))

        for nombre_sensor, valores in sensores.items():
            if isinstance(valores, dict) and "valor" in valores:
                sensor_row = conn.execute(sa.text("""
                    SELECT id_sensores FROM sensores
                    WHERE nombre = :nombre AND id_sistema = :id_sistema
                """), {'nombre': nombre_sensor, 'id_sistema': id_sistema}).fetchone()
                if sensor_row:
                    conn.execute(sa.text("""
                        INSERT INTO lecturas (id_sensor, valor, fecha_hora)
                        VALUES (:id_sensor, :valor, :fecha)
                    """), {
                        'id_sensor': sensor_row[0],
                        'valor': valores["valor"],
                        'fecha': fecha_hora
                    })

        # Guardar evento_control (ej. bomba)
        actuadores = data.get("actuadores", {})
        if "bomba" in actuadores:
            act_row = conn.execute(sa.text("""
                SELECT id_actuador FROM actuadores
                WHERE nombre = 'BOMBA' AND id_sistema = :id_sistema
            """), {'id_sistema': id_sistema}).fetchone()
            if act_row:
                conn.execute(sa.text("""
                    INSERT INTO eventos_control (id_actuador, accion, fecha_hora, id_usuario)
                    VALUES (:id_actuador, :accion, :fecha, NULL)
                """), {
                    'id_actuador': act_row[0],
                    'accion': 'ON' if actuadores["bomba"] else 'OFF',
                    'fecha': fecha_hora
                })

        conn.commit()

def procesar_alarma(data: dict):
    logger.info(f"[ALARMA] Nivel {data.get('nivel')} de dispositivo {data.get('device')} - {data.get('mensaje')}")
