#include "sensores.h"
#include "expansion.h"

// Objetos para sensores
DHTesp dhtSensor;  // Cambiado a DHTesp por compatibilidad con DHTesp.h
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
Adafruit_INA219 ina219;

// Variables para flujo de agua
volatile int pulsosFlujo = 0;
float factorCalibracion = 4.5;  // Ejemplo: factor de calibración para el sensor de flujo

// Variables de sensores
float temperaturaAgua = 0.0;
float temperaturaAire = 0.0;
float humedadAire = 0.0;  // Corregido el nombre de la variable
float nivelAgua = 0.0;
float flujoAgua = 0.0;
int luz = 0;
int gas = 0;
float corriente = 0.0;
float voltaje = 0.0;
float potencia = 0.0;
float ph = 0.0;  // Corregido nombre para coincidir con el uso en leerPH()

// Variables para activar/desactivar sensores
bool sensorTempAguaActivo = true;
bool sensorAireActivo = true;
bool sensorHumedadActivo = true;
bool sensorNivelAguaActivo = true;
bool sensorFlujoActivo = true;
bool sensorLuzActivo = true;
bool sensorGasActivo = true;
bool sensorCorrienteActivo = true;
bool sensorPHActivo = true;

// Inicialización de sensores
void inicializarSensores() {
    dhtSensor.setup(DHT_PIN, DHTesp::DHT11);  // Inicialización correcta para DHTesp
    sensors.begin();
    ina219.begin();
    
    // Configurar interrupción para el sensor de flujo si es necesario
    if (sensorFlujoActivo) {
        pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), contadorPulsos, FALLING);
    }
    
    // Inicialización del sensor ultrasónico
    pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);
    
    expansion_init();
}

// Función para la interrupción del sensor de flujo
void IRAM_ATTR contadorPulsos() {
    pulsosFlujo++;
}

// Sensores con validación de activación/desactivación

bool leerTemperaturaAgua() {
    if (!sensorTempAguaActivo) return false;

    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    if (temp < -50.0) {
        Serial.println("Error: Sensor de temperatura de agua desconectado.");
        return false;
    }
    temperaturaAgua = temp;
    Serial.print("Temperatura del agua: ");
    Serial.print(temperaturaAgua);
    Serial.println(" °C");
    return true;
}

bool leerDHT() {
    if (!sensorAireActivo && !sensorHumedadActivo) return false;

    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    if (isnan(data.temperature) || isnan(data.humidity)) {
        Serial.println("Error: Fallo al leer DHT.");
        return false;
    }
    temperaturaAire = data.temperature;
    humedadAire = data.humidity;
    Serial.print("Temperatura del aire: ");
    Serial.print(temperaturaAire);
    Serial.print(" °C - Humedad: ");
    Serial.print(humedadAire);
    Serial.println(" %");
    return true;
}

bool leerNivelAgua() {
    if (!sensorNivelAguaActivo) return false;

    long duration;

    // Disparar la señal ultrasónica
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

    // Recibir el eco con timeout para evitar bloqueos
    duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 100000);

    // Si no se recibe un eco válido, mostrar error
    if (duration == 0) {
        Serial.println("⚠️ Error: No se recibió el eco del sensor de nivel.");
        return false;
    } else {
        nivelAgua = duration * 0.034 / 2;  // Convertir tiempo en distancia (cm)
        Serial.print("📏 Nivel de agua: ");
        Serial.print(nivelAgua);
        Serial.println(" cm");
        return true;
    }
}

bool calcularFlujoAgua() {
    if (!sensorFlujoActivo) return false;

    flujoAgua = pulsosFlujo * factorCalibracion;
    Serial.print("Flujo de agua: ");
    Serial.print(flujoAgua);
    Serial.println(" L/min");
    pulsosFlujo = 0;  // Reiniciar conteo para la siguiente lectura
    return true;
}

bool leerLuz() {
    if (!sensorLuzActivo) return false;

    // Para un sensor digital de luz (HIGH = no hay luz, LOW = hay luz) 
    // Este comportamiento puede variar según el sensor, ajústalo según sea necesario
    int lectura = digitalRead(LDR_PIN);
    
    // Inverir la lógica si es necesario según tu sensor
    // Si lectura es LOW, hay luz (1), si es HIGH no hay luz (0)
    luz = (lectura == LOW) ? 100 : 0;  // 100% si hay luz, 0% si no hay luz
    
    Serial.print("Estado de luz: ");
    if (luz > 0) {
        Serial.println("Detectada");
    } else {
        Serial.println("No detectada");
    }
    return true;
}

bool leerGas() {
    if (!sensorGasActivo) return false;

    int lectura = analogRead(GAS_SENSOR_PIN);
    gas = map(lectura, 0, 4095, 0, 100);  // También como porcentaje
    Serial.print("Nivel de gas: ");
    Serial.print(gas);
    Serial.println(" % (peligro si > 60%)");
    return true;
}

bool leerCorriente() {
    if (!sensorCorrienteActivo) return false;

    // Usando INA219 en lugar del pin analógico
    corriente = ina219.getCurrent_mA() / 1000.0;  // Convertir de mA a A
    voltaje = ina219.getBusVoltage_V();
    potencia = ina219.getPower_mW() / 1000.0;  // Convertir de mW a W
    
    Serial.print("Corriente: ");
    Serial.print(corriente);
    Serial.print(" A, Voltaje: ");
    Serial.print(voltaje);
    Serial.print(" V, Potencia: ");
    Serial.print(potencia);
    Serial.println(" W");
    return true;
}

bool leerPH() {
    if (!sensorPHActivo) return false;

    int lectura = analogRead(PH_SENSOR_PIN);
    ph = lectura * 14.0 / 4095.0;  // Convertimos lectura en valor de pH
    Serial.print("pH del agua: ");
    Serial.println(ph);
    return true;
}