#include "SensorManager.h"
#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>

//----------------------------------
// Funciones auxiliares para caudalímetro
//----------------------------------
static float calcularCaudal(int pulsos, float factorPulsosPorLitro, float intervaloSeg) {   // factorPulsosPorLitro: p.ej. 450 pulsos/L
  return ( (float)pulsos / factorPulsosPorLitro ) * (60.0f / intervaloSeg);                 // caudal (L/min) = (pulsos / factor) * (60 / intervaloSeg)
}
static float calcularVolumen(int pulsos, float factorPulsosPorLitro) {                      // volumen (L) = pulsos / factor
  return (float)pulsos / factorPulsosPorLitro;
}

//------------------------------
// Constructor
//------------------------------
SensorManager::SensorManager(
    uint8_t i2c_sda, uint8_t i2c_scl,
    uint8_t pin_dht11,
    uint8_t pin_bh1750_sda, uint8_t pin_bh1750_scl,
    uint8_t pin_sr04_trig, uint8_t pin_sr04_echo,
    uint8_t pin_caudalimetro, float factor_caudal,
    uint8_t pin_co2
)
  : bmp_(PIN_I2C_SDA, PIN_I2C_SCL)                                  // Ahora acepta pines I2C
  , dht_(PIN_DHT11)                                                 // DHT11 con pin
  , bh1750_(PIN_BH1750_SDA, PIN_BH1750_SCL)                         // BH1750 con I2C configurable
  , sr04_(PIN_HC_SR04_TRIG, PIN_HC_SR04_ECHO)                       // Pines en el constructor
  , caudalimetro_(PIN_CAUDALIMETRO, FACTOR_CAUDAL)                  // Caudalímetro parametrizado
  , rtc_()                                                          // RTC usa Wire por defecto, pines por begin()
  , co2_(PIN_CO2)                                                   // Sensor CO2 con pin configurable
  , moduloEnergia_(Wire)                                            // Módulo energía acepta Wire

  // Valores arbitrarios iniciales; luego se sobrescriben con NVS
  , tempBMP_(NAN)
  , presionBMP_(NAN)
  , alturaBMP_(NAN)
  , tempDHT_(NAN)
  , humDHT_(NAN)
  , luxBH_(NAN)
  , distSR_(NAN)
  , pulsosCapturados_(0)
  , caudal_(NAN)
  , volumenAcum_(NAN)
  , horaRTC_("desconocida")
  , co2PPM_(NAN)
  , voltaje_(NAN)
  , corriente_(NAN)
  , potencia_(NAN)
  
  // Umbrales por defecto
  , umbralTempMax_(40.0f)
  , umbralHumMin_(30.0f)
  , umbralPresionMin_(980.0f)
  , umbralLuxMin_(100.0f)
  , umbralDistMax_(200.0f)
  , umbralCaudalMin_(0.5f)
  , umbralCO2Max_(1000.0f)
  , umbralVoltMin_(3.0f)
  , umbralCorrMin_(0.1f)
  , intervaloLecturaMs_(5000)
  , ultimaLecturaMs_(0)
  , estado_(Estado::Inactivo)
{
}

//------------------------------
// comenzar(): inicializa todo
//------------------------------
void SensorManager::comenzar() {
  // 1) Cargar umbrales e intervalo desde NVS
  prefs_.begin("config", true);
  umbralTempMax_     = prefs_.getFloat("umbralTempMax",     umbralTempMax_);
  umbralHumMin_      = prefs_.getFloat("umbralHumMin",      umbralHumMin_);
  umbralPresionMin_  = prefs_.getFloat("umbralPresionMin",  umbralPresionMin_);
  umbralLuxMin_      = prefs_.getFloat("umbralLuxMin",      umbralLuxMin_);
  umbralDistMax_     = prefs_.getFloat("umbralDistMax",     umbralDistMax_);
  umbralCaudalMin_   = prefs_.getFloat("umbralCaudalMin",   umbralCaudalMin_);
  umbralCO2Max_      = prefs_.getFloat("umbralCO2Max",      umbralCO2Max_);
  umbralVoltMin_     = prefs_.getFloat("umbralVoltMin",     umbralVoltMin_);
  umbralCorrMin_     = prefs_.getFloat("umbralCorrMin",     umbralCorrMin_);
  intervaloLecturaMs_= prefs_.getUInt("intervaloSensoresMs", intervaloLecturaMs_);
  float factorCalib  = prefs_.getFloat("factorCaudal", 450.0f);

  prefs_.end();

  // 2) Inicializar cada sensor; si alguno falla críticamente, pasa a Estado::Error
  Wire.begin(); // [I2C genérico] Inicializamos Wire para BMP280, BH1750, Módulo de Energía, RTC

  // 2.2) BMP280
  if (!bmp_.begin()) {
    Serial.println("[SensorMgr] [ERROR] no pudo inicializar BMP280");
    estado_ = Estado::Error;
    return;
  }

  // 2.3) DHT11
  dht_.begin(); // no devuelve error; lecturas posteriores detectan NAN

  // 2.4) BH1750
  if (!bh1750_.begin()) {
    Serial.println("[SensorMgr] [ERROR] no pudo inicializar BH1750");
    estado_ = Estado::Error;
    return;
  }

  // 2.5) HC-SR04 (pines definidos en el constructor)
  sr04_.begin(PIN_HC_SR04_TRIG, PIN_HC_SR04_ECHO);

  // 2.6) Caudalímetro (no requiere begin; simplemente usar ISR y setPulsosCaudal)

  // 2.7) RTC DS3231
  rtc_.begin(); // si no arranca, getTimeString detecta fallo luego

  // 2.8) SensorCO2
  co2_.begin(); // imprime calibración en Serial; siempre debería completar

  // 2.9) Módulo de Energía
  moduloEnergia_.begin();
  if (!moduloEnergia_.isCommunicationOK()) {
    Serial.println("[SensorMgr] [ERROR] no pudo comunicarse con Módulo de Energía");
    estado_ = Estado::Error;
    return;
  }

  // Si llegamos aquí, todo OK → pasamos a Estado::Leyendo
  estado_ = Estado::Leyendo;
  ultimaLecturaMs_ = millis();
  Serial.println("[SensorMgr] [INFO] Sensores inicializados correctamente");
}

//------------------------------
// loop(): lee cuando toca
//------------------------------
void SensorManager::loop() {
  if (estado_ != Estado::Leyendo) return;

  unsigned long ahora = millis();
  if (ahora - ultimaLecturaMs_ < intervaloLecturaMs_) {
    return; // aún no toca leer
  }
  ultimaLecturaMs_ = ahora;

  leerSensores();
}

//------------------------------
// leerSensores(): lee todos
//------------------------------
void SensorManager::leerSensores() {
  // ------------------------------------------------------------------
  // 1) RTC DS3231: obtenemos hora
  // ------------------------------------------------------------------
  {
    String sHora = rtc_.getTimeString();
    if (sHora == "RTC no disponible" || sHora == "Hora no válida") {
      horaRTC_ = "";
    } else {
      horaRTC_ = sHora;
    }
  }

  // ------------------------------------------------------------------
  // 2) BMP280: temperatura, presión, altitud
  // ------------------------------------------------------------------
  if (bmp_.isConnected()) {
    tempBMP_    = bmp_.readTemperature();
    presionBMP_ = bmp_.readPressure();
    alturaBMP_  = bmp_.readAltitude(1013.25f);
  } else {
    tempBMP_    = NAN;
    presionBMP_ = NAN;
    alturaBMP_  = NAN;
  }

  // ------------------------------------------------------------------
  // 3) DHT11: temp y humedad
  // ------------------------------------------------------------------
  {
    float tDHT = dht_.readTemperature();
    float hDHT = dht_.readHumidity();
    if (isnan(tDHT) || isnan(hDHT)) {
      tempDHT_ = NAN;
      humDHT_  = NAN;
    } else {
      tempDHT_ = tDHT;
      humDHT_  = hDHT;
    }
  }

  // ------------------------------------------------------------------
  // 4) BH1750: luminosidad
  // ------------------------------------------------------------------
  {
    float lux = bh1750_.readLightLevel();
    if (lux < 0) {
      luxBH_ = NAN;
    } else {
      luxBH_ = lux;
    }
  }

  // ------------------------------------------------------------------
  // 5) HC-SR04: distancia
  // ------------------------------------------------------------------
  distSR_ = sr04_.read(); // devolverá -1 si hubo timeout

  // ------------------------------------------------------------------
  // 6) Caudalímetro: caudal y volumen
  // ------------------------------------------------------------------
  {
    // Leemos el factor de calibración si lo queremos dinámico:
    float factorCalib = prefs_.getFloat("factorCaudal", 450.0f);
    float intervaloSeg = (float)intervaloLecturaMs_ / 1000.0f;
    if (pulsosCapturados_ >= 0) {
      caudal_      = calcularCaudal(pulsosCapturados_, factorCalib, intervaloSeg);
      volumenAcum_ = calcularVolumen(pulsosCapturados_, factorCalib);
      pulsosCapturados_ = 0;
    } else {
      caudal_      = NAN;
      volumenAcum_ = NAN;
      pulsosCapturados_ = 0;
    }
  }

  // ------------------------------------------------------------------
  // 7) SensorCO2: PPM
  // ------------------------------------------------------------------
  {
    float ppm = co2_.readPPM();
    if (ppm < 0 || isnan(ppm)) {
      co2PPM_ = NAN;
    } else {
      co2PPM_ = ppm;
    }
  }

  // ------------------------------------------------------------------
  // 8) Módulo de Energía: voltaje, corriente, potencia
  // ------------------------------------------------------------------
  {
    if (moduloEnergia_.isCommunicationOK()) {
      float v = moduloEnergia_.getVoltage();
      float i = moduloEnergia_.getCurrent();
      if (v < 0.0f || i < 0.0f) {
        voltaje_   = NAN;
        corriente_ = NAN;
        potencia_  = NAN;
      } else {
        voltaje_   = v;
        corriente_ = i;
        potencia_  = moduloEnergia_.getPower();
        // potencia_ será -1.0 si hay error
        if (potencia_ < 0.0f) {
          potencia_ = NAN;
        }
      }
    } else {
      voltaje_   = NAN;
      corriente_ = NAN;
      potencia_  = NAN;
    }
  }
}

//------------------------------
// obtenerJson(): arma JSON
//------------------------------
String SensorManager::obtenerJson() {
  jsonDoc_.clear();

  // 1) Timestamp “genérico”
  // Si tienes un RTC sincronizado, podrías usar rtc_.getTimeString()
  // aquí mismo en lugar de millis(). Por simplicidad usamos millis().
  unsigned long t = millis();
  char bufHora[32];
  snprintf(bufHora, sizeof(bufHora), "%lu", t);
  jsonDoc_["timestamp"] = bufHora;

  // ------------------------------------------------------------------
  // 2) Nodo “RTC”
  // ------------------------------------------------------------------
  {
    JsonObject jRTC = jsonDoc_.createNestedObject("RTC");
    if (horaRTC_.length() > 0) {
      jRTC["hora"]   = horaRTC_;
      jRTC["estado"] = "OK";
    } else {
      jRTC["hora"]   = "";
      jRTC["estado"] = "ERR";
    }
  }

  // ------------------------------------------------------------------
  // 3) Nodo “BMP280”
  // ------------------------------------------------------------------
  {
    JsonObject jBMP = jsonDoc_.createNestedObject("BMP280");
    if (!isnan(tempBMP_) && !isnan(presionBMP_)) {
      const char* est = (tempBMP_ <= umbralTempMax_ && presionBMP_ >= umbralPresionMin_)
                            ? "OK"
                            : "ERR";
      // Registramos temperatura
      registrarLectura("BMP280", tempBMP_, "°C", est, jBMP);
      // Agregamos presión y altura
      jBMP["presion"] = presionBMP_;
      jBMP["altura"]  = alturaBMP_;
    } else {
      registrarLectura("BMP280", NAN, "", "ERR", jBMP);
    }
  }

  // ------------------------------------------------------------------
  // 4) Nodo “DHT11”
  // ------------------------------------------------------------------
  {
    JsonObject jDHT = jsonDoc_.createNestedObject("DHT11");
    if (!isnan(tempDHT_) && !isnan(humDHT_)) {
      const char* est = (tempDHT_ <= umbralTempMax_ && humDHT_ >= umbralHumMin_)
                            ? "OK"
                            : "ERR";
      registrarLectura("DHT11", tempDHT_, "°C", est, jDHT);
      jDHT["hum"] = humDHT_;
    } else {
      registrarLectura("DHT11", NAN, "", "ERR", jDHT);
    }
  }

  // ------------------------------------------------------------------
  // 5) Nodo “BH1750”
  // ------------------------------------------------------------------
  {
    JsonObject jBH = jsonDoc_.createNestedObject("BH1750");
    if (!isnan(luxBH_)) {
      const char* est = (luxBH_ >= umbralLuxMin_) ? "OK" : "ERR";
      registrarLectura("BH1750", luxBH_, "lux", est, jBH);
    } else {
      registrarLectura("BH1750", NAN, "", "ERR", jBH);
    }
  }

  // ------------------------------------------------------------------
  // 6) Nodo “HC-SR04”
  // ------------------------------------------------------------------
  {
    JsonObject jSR = jsonDoc_.createNestedObject("HC-SR04");
    if (distSR_ >= 0) {
      const char* est = (distSR_ <= umbralDistMax_) ? "OK" : "ERR";
      registrarLectura("HC-SR04", distSR_, "cm", est, jSR);
    } else {
      registrarLectura("HC-SR04", NAN, "", "ERR", jSR);
    }
  }

  // ------------------------------------------------------------------
  // 7) Nodo “Caudal”
  // ------------------------------------------------------------------
  {
    JsonObject jC = jsonDoc_.createNestedObject("Caudal");
    if (!isnan(caudal_) && !isnan(volumenAcum_)) {
      const char* est = (caudal_ >= umbralCaudalMin_) ? "OK" : "ERR";
      registrarLectura("Caudal", caudal_, "L/min", est, jC);
      jC["volumen"] = volumenAcum_;
    } else {
      registrarLectura("Caudal", NAN, "", "ERR", jC);
    }
  }

  // ------------------------------------------------------------------
  // 8) Nodo “CO2”
  // ------------------------------------------------------------------
  {
    JsonObject jCO2 = jsonDoc_.createNestedObject("CO2");
    if (!isnan(co2PPM_)) {
      const char* est = (co2PPM_ <= umbralCO2Max_) ? "OK" : "ERR";
      registrarLectura("CO2", co2PPM_, "ppm", est, jCO2);
    } else {
      registrarLectura("CO2", NAN, "", "ERR", jCO2);
    }
  }

  // ------------------------------------------------------------------
  // 9) Nodo “Energia”
  // ------------------------------------------------------------------
  {
    JsonObject jE = jsonDoc_.createNestedObject("Energia");
    if (!isnan(voltaje_) && !isnan(corriente_)) {
      const char* est = (voltaje_ >= umbralVoltMin_ && corriente_ >= umbralCorrMin_)
                            ? "OK"
                            : "ERR";
      // Voltaje
      if (!isnan(voltaje_)) jE["voltaje"] = voltaje_;
      // Corriente
      if (!isnan(corriente_)) jE["corriente"] = corriente_;
      // Potencia
      if (!isnan(potencia_))  jE["potencia"]  = potencia_;
      jE["estado"] = est;
    } else {
      registrarLectura("Energia", NAN, "", "ERR", jE);
    }
  }

  // Convertir documento a String y devolver
  String salida;
  serializeJson(jsonDoc_, salida);
  return salida;
}

//------------------------------
// registrarLectura(): ayuda a volcar un float al JSON
//------------------------------
void SensorManager::registrarLectura(const char* nombre,
                                     float valor,
                                     const char* unidad,
                                     const char* estado,
                                     JsonObject& nodo) {
  // Si el valor es válido, incluimos “valor” y “unidad”
  if (!isnan(valor)) {
    nodo["valor"]  = valor;
    nodo["unidad"] = unidad;
  }
  nodo["estado"] = estado;
}

//------------------------------
// setPulsosCaudal(): llamado desde ISR o main antes de loop()
//------------------------------
void SensorManager::setPulsosCaudal(int pulsos) {
  pulsosCapturados_ = pulsos;
}

//------------------------------
// getEstado()
//------------------------------
SensorManager::Estado SensorManager::getEstado() const {
  return estado_;
}
