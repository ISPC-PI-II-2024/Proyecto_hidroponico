import json
import paho.mqtt.client as mqtt

# === Configuración del broker y topic ===
MQTT_BROKER   = "telecomunicaciones.ddns.net"
MQTT_PORT     = 2480
MQTT_USER     = "telecomunicaciones"
MQTT_PASSWORD = "cohorte*2024"
MQTT_TOPIC    = "dispositivo/info"

# === Payload idéntico al ejemplo ===
payload = {
    "gatewayId": "esp32-F008D1432C5C",
    "timestamp": "2025-5-21 3:11:46",
    "nodes": [
        {
            "deviceId": "esp32-F008D1432C5C",
            "timestamp": "2025-5-21 3:11:46",
            "transmitter": True,
            "receiver": True,
            "sensors": {
                "tempAgua": { "pin": "GPIO25", "type": "DS18B20" },
                "tempAire": { "pin": "GPIO33", "type": "DHT11\u00000\u0000" },
                "humedad": { "pin": "GPIO33", "type": "DHT11\u00000" },
                "nivel": { "pin": "GPIO32", "type": "HC-SR" },
                "flujo": { "pin": "GPIO34", "type": "YF-S2" },
                "luz": { "pin": None }
            }
        }
    ]
}

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado correctamente al broker.")
    else:
        print(f"Error de conexión. Código: {rc}")

def main():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    client.on_connect = on_connect

    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_start()
    
    # Espera a la conexión (opcional, para asegurar envío)
    import time
    time.sleep(1)
    
    client.publish(MQTT_TOPIC, json.dumps(payload))
    print("Payload enviado correctamente.")

    # Espera para asegurar envío y luego termina
    time.sleep(1)
    client.loop_stop()
    client.disconnect()

if __name__ == "__main__":
    main()
