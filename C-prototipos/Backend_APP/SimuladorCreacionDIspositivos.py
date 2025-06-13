import json
import random
import time
from datetime import datetime
import paho.mqtt.client as mqtt

# ——— Configuración MQTT ———
BROKER   = "telecomunicaciones.ddns.net"
PORT     = 2480
USERNAME = "telecomunicaciones"
PASSWORD = "cohorte*2024"
TOPIC    = "dispositivo/info"

# Rangos y medias para distribución gaussiana de cada sensor
SENSOR_CFG = {
    "tempAgua": {"type": "DS18B20", "mean": 20.0, "sigma": 2.0},
    "tempAire": {"type": "DHT11",   "mean": 25.0, "sigma": 2.0},
    "humedad":  {"type": "DHT11",   "mean":  50.0, "sigma":10.0},
    "nivel":    {"type": "HC-SR",   "mean":100.0, "sigma":20.0},
    "flujo":    {"type": "YF-S2",   "mean":  5.0, "sigma": 2.0},
    "luz":      {"type": "LDR",     "mean":300.0, "sigma":50.0},
}

def build_payload(device_idx: int) -> str:
    # 1) deviceId secuencial: 00 a 09
    suffix = f"{device_idx:02d}"
    device_id = f"esp32-F008D1432C{suffix}"

    # 2) timestamp local sin ceros a la izquierda
    now = datetime.now()
    ts = f"{now.year}-{now.month}-{now.day} {now.hour}:{now.minute}:{now.second}"

    # 3) montar diccionario de sensores
    sensors = {}
    for name, cfg in SENSOR_CFG.items():
        # pin aleatorio entre 0 y 39
        pin_num = random.randint(0, 39)
        pin = f"GPIO{pin_num}"
        # valor gaussiano y recorte de realismo
        raw = random.gauss(cfg["mean"], cfg["sigma"])
        if name == "humedad":
            val = max(0.0, min(100.0, raw))
        else:
            val = round(raw, 2)
        sensors[name] = {
            "pin": pin,
            "type": cfg["type"],
            "value": val
        }

    # 4) payload completo
    payload = {
        "gatewayId": device_id,
        "timestamp": ts,
        "nodes": [
            {
                "deviceId": device_id,
                "timestamp": ts,
                "transmitter": True,
                "receiver": True,
                "sensors": sensors
            }
        ]
    }
    return json.dumps(payload)

def main():
    client = mqtt.Client()
    client.username_pw_set(USERNAME, PASSWORD)
    client.connect(BROKER, PORT)

    for idx in range(10):
        pl = build_payload(idx)
        result = client.publish(TOPIC, pl, qos=0)
        # opcional: espera breve entre mensajes
        time.sleep(0.1)
        # comprobar envío (opcional):
        status = result[0]
        if status == mqtt.MQTT_ERR_SUCCESS:
            print(f"[{idx:02d}] Publicado correctamente")
        else:
            print(f"[{idx:02d}] Error al publicar: {status}")

    client.disconnect()

if __name__ == "__main__":
    main()