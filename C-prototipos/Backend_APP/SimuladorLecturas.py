import time
import random
import json
from datetime import datetime
import paho.mqtt.client as mqtt

# ——— Configuración ———
BROKER_HOST   = "telecomunicaciones.ddns.net"
BROKER_PORT   = 2480
TOPIC         = "sensores/datos"
USERNAME      = "telecomunicaciones"
PASSWORD      = "cohorte*2024"

# Array de gateways para testeo
GATEWAY_IDS = [
    "esp32-F008D1432C00",
    "esp32-F008D1432C01",
    "esp32-F008D1432C02",
    "esp32-F008D1432C03",
    "esp32-F008D1432C04",
    "esp32-F008D1432C05",
    "esp32-F008D1432C06",
    "esp32-F008D1432C07",
    "esp32-F008D1432C08",
    "esp32-F008D1432C09",
]

# Especificaciones Gaussianas por sensor: (media, desviación típica)
SENSOR_SPECS = {
    "temperaturaAgua": (20.0,  5.0),
    "temperaturaAire": (22.0,  3.0),
    "humedad":         (50.0, 10.0),
    "nivelAgua":       (100.0, 50.0),
    "flujoAgua":       (5.0,   2.0),
    "luz":             (300.0,100.0),
    "gas":             (1.0,   1.0),
    "corriente":       (0.1,   0.05),
    "voltaje":         (3.3,   0.2),
    "potencia":        (1.0,   0.5),
    "phValor":         (7.0,   0.5),
}

# Estados de los actuadores (puedes dejarlos constantes o también aleatorizarlos)
CONTROLS_TEMPLATE = {
    "bomba": False,
    "luces": False,
    "alarma": False,
    "modoAutomatico": True
}

# ——— Funciones ———
def generate_payload(gateway_id: str) -> dict:
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    # Generar lecturas gaussianas y truncar a ≥0
    sensors = {}
    for name, (mu, sigma) in SENSOR_SPECS.items():
        v = random.gauss(mu, sigma)
        sensors[name] = round(max(v, 0), 4)
    sensors["hora"] = time.time()

    return {
        "gatewayId": gateway_id,
        "timestamp": now,
        "nodes": [
            {
                "deviceId":   gateway_id,
                "timestamp":  now,
                "transmitter": True,
                "receiver":    True,
                "sensors":     sensors,
                "controls":    CONTROLS_TEMPLATE
            }
        ]
    }

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"Conectado a MQTT ({BROKER_HOST}:{BROKER_PORT})")
    else:
        print(f"Error al conectar, código {rc}")

def main():
    client = mqtt.Client()
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.connect(BROKER_HOST, BROKER_PORT, keepalive=60)
    client.loop_start()

    idx = 0
    total = len(GATEWAY_IDS)
    try:
        while True:
            gw = GATEWAY_IDS[idx % total]
            payload = generate_payload(gw)
            client.publish(TOPIC, json.dumps(payload), qos=0)
            print(f"[{datetime.now().strftime('%H:%M:%S')}] Publicado para {gw}")
            idx += 1
            time.sleep(5)
    except KeyboardInterrupt:
        print("Test interrumpido por usuario")
    finally:
        client.loop_stop()
        client.disconnect()

if __name__ == "__main__":
    main()
