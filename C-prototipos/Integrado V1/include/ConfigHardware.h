// ConfigHardware.h
#pragma once

// Sensores analógicos
#define PIN_LM35           36
#define PIN_PH             34
#define PIN_EC             35
#define PIN_CORRIENTE      32
#define PIN_GAS            39

// Sensores digitales
#define PIN_DHT11          4
#define PIN_DS18B20        5
#define PIN_FLOTADOR       14
#define PIN_HCSR04_TRIG    12
#define PIN_HCSR04_ECHO    13
#define PIN_RELE_BOMBA     25
#define PIN_CAUDALIMETRO   26
#define PIN_ENERGIA        33
#define PIN_BUZZER         27
#define PIN_LLUVIA         18

// Sensores I2C
#define PIN_I2C_SDA        21
#define PIN_I2C_SCL        22
#define PIN_BH1750_SDA     PIN_I2C_SDA
#define PIN_BH1750_SCL     PIN_I2C_SCL
#define PIN_BME280_SDA     PIN_I2C_SDA
#define PIN_BME280_SCL     PIN_I2C_SCL
#define PIN_RTC_SDA        PIN_I2C_SDA
#define PIN_RTC_SCL        PIN_I2C_SCL

#define PIN_LED_VERDE      2
#define PIN_LED_ROJO       15

// Constantes generales
#define FACTOR_CAUDAL      450.0f
#define PIN_CO2            19  
