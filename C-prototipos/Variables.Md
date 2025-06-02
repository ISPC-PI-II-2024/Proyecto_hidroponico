# Proyecto Hidropónico: Variables y Base de Datos

Este sistema hidropónico estará integrado con **Node-RED** para la gestión de datos en tiempo real y **MariaDB** para el almacenamiento estructurado de información.
## Variables de Sensores y Control

- **`float temperaturaAgua`** → Registra la temperatura del agua en grados Celsius.
- **`float temperaturaAire`** → Mide la temperatura ambiental.
- **`float humedadAire`** → Indica la humedad relativa en el ambiente.
- **`float nivelAgua`** → Monitorea el nivel del agua en el sistema.
- **`float flujoAgua`** → Indica la velocidad de circulación del agua.
- **`int luz`** → Registra la cantidad de luz recibida por las plantas.
- **`int gas`** → Indica la concentración de gases en el ambiente.
- **`float corriente`**, **`float voltaje`**, **`float potencia`** → Variables eléctricas para el monitoreo energético.
- **`float ph`** → Mide el pH del agua para garantizar la absorción óptima de nutrientes.
- **`int lluvia`** → Indica si ha habido precipitaciones externas.
- **`int bomba`** → Estado de activación de la bomba de circulación.

## Variables de RTC (Real-Time Clock)

Para el registro preciso de datos en **MariaDB**, se incorporan variables de RTC:

- **`int año`** → Año actual.
- **`int mes`** → Mes actual.
- **`int día`** → Día actual.
- **`int hora`** → Hora actual.
- **`int minuto`** → Minuto actual.
- **`int segundo`** → Segundo actual.

## Estructura de la Base de Datos en MariaDB

### Creación de la Base de Datos y Tabla

```
CREATE DATABASE Hidroponico;
USE Hidroponico;

CREATE TABLE Sensores (
    id INT AUTO_INCREMENT PRIMARY KEY,
    temperaturaAgua FLOAT,
    temperaturaAire FLOAT,
    humedadAire FLOAT,
    nivelAgua FLOAT,
    flujoAgua FLOAT,
    luz INT,
    gas INT,
    corriente FLOAT,
    voltaje FLOAT,
    potencia FLOAT,
    ph FLOAT,
    lluvia INT,
    bomba INT,
    año INT,
    mes INT,
    día INT,
    hora INT,
    minuto INT,
    segundo INT,
    fecha_registro TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);