# Proyecto_hidroponico<p align="center">
  <img src="E-Assets/logo.png" width="200" alt="ISPC Hydroponics logo"/>
</p>

<h1 align="center">ISPC Hydroponics</h1>

<p align="center">
  🌱 Cultivando el futuro con tecnología inteligente 💧
</p>

---

## 📌 Descripción

**ISPC Hydroponics** es un proyecto IoT colaborativo desarrollado por estudiantes del Instituto Superior Politecnico Cordoba, correspondientes a la materia Proyecto Integrador 2 de la Tecnicatura Superior en Telecomunicaciones.
En el mismo se busca aplicar tecnología para monitorear y automatizar un sistema de cultivo hidropónico. 
El objetivo es construir una solución inteligente, ecológica y accesible que combine sensores, microcontroladores y visualización en tiempo real.

---

## 🧑‍💻 Integrantes

| Nombre                        | GitHub                                 |
|------------------------------|----------------------------------------|
| Romina Vanesa Huk            | [@RoHu17](https://github.com/RoHu17) |
| Emma Vilma Gutierrez         | [@emygut](https://github.com/emygut) |
| Macarena Aylèn Carballo      | [@MacarenaAC](https://github.com/MacarenaAC) |
| Jose Luis Marquez            | [@marquezjose](https://github.com/marquezjose) |
| Fernando Gimenez Coria       | [@FerCbr](https://github.com/FerCbr) |
| Nahuel Lucas Velez           | [@Lucasmurua19](https://github.com/Lucasmurua19) |
| Juan Diego Gonzalez Antoniazzi | [@JDGA1997](https://github.com/JDGA1997) |
| Lisandro Juncos Varalda      | [@Lisandro-05](https://github.com/Lisandro-05) |
| Joaquin Emiliano Garzón      | [@Joacogarzonn](https://github.com/Joacogarzonn) |
| Vittorio Durigutti           | [@vittoriodurigutti](https://github.com/vittoriodurigutti) |
| Leandro Roldan               | [@pleroldan](https://github.com/pleroldan) |
| Luciano Lujan                | [@lucianoilujan](https://github.com/lucianoilujan) |
| Tiziano Paez                 | [@tpaez](https://github.com/tpaez) |
| Raul Jara                    | [@r-j28](https://github.com/r-j28) |
| Joaquin Zalazar              | [@breaakerr](https://github.com/breaakerr) |
| Diego Ares                   | [@diegote7](https://github.com/diegote7) |
| Paola Pantoja                | [@PaolaaPantoja](https://github.com/PaolaaPantoja) |

---

## 🧰 Tecnologías usadas

- ESP32 + sensores físicos
- Node-RED
- MQTT (Mosquitto)
- MariaDB
- Grafana
- Portainer (orquestación Docker)
- GitHub (Scrum / Kanban)
- Power BI (análisis final)
- API REST en Node.js/Express
- Visualización en tiempo real

---

## 📂 Organización

```
📁 /Requisitos         → los requisitos del proyecto  
📁 /Investigacion      → Archivos de investigacion, procedimientos, apuntes, datasheets 
📁 /Prototipos         → Prototipos, pruebas unitarios, integraciones, etc 
📁 /Presentacion       → Documentacion de presentacion, videos, diapositivas, manuales de uso
📁 /assets       → Imágenes, logo, multimedia  
📄 README.md     → Este archivo  
📄 LICENSE       → Licencia del proyecto
```

## 🚀 Objetivos

- Monitorear variables ambientales (pH, humedad, temperatura).
- Controlar actuadores mediante Node-RED.
- Visualizar datos históricos en Grafana.
- Almacenar información en MariaDB.
- Integrar toda la arquitectura en una red Docker.
- Desarrollar e implementar una API REST para el acceso programático a los datos.

## 🧰 Tecnologías usadas

## 📂 Estructura de documentación

| Sección | Descripción |
|--------|-------------|
| [1 - Introducción](docs/1-introduccion.md) | Presentación del proyecto y objetivos |
| [2 - Instalación](docs/2-instalacion.md) | Guía paso a paso para levantar el entorno |
| [3 - Arquitectura](docs/3-arquitectura.md) | Diagrama general del sistema IoT |
| [4 - Node-RED](docs/4-node-red.md) | Flujos de automatización y ejemplos |
| [5 - Grafana](docs/5-grafana.md) | Dashboards de visualización |
| [6 - Base de datos](docs/6-bd-mariadb.md) | Esquema de la BD y acceso |
| [7 - MQTT Broker](docs/7-mqtt-broker.md) | Configuración y pruebas del broker MQTT |
| [8 - API REST](docs/8-api.md) | Descripción y endpoints de la API |

## 🧑‍💻 Accesos a entorno

- [Node-RED](http://telecomunicaciones.ddns.net:8080/nodered/)
- [Dashboard UI](http://telecomunicaciones.ddns.net:8080/nodered/ui)
- [Grafana](http://telecomunicaciones.ddns.net:8080/grafana/)
- [Portainer](http://telecomunicaciones.ddns.net:8080/portainer/)
- [MariaDB](http://telecomunicaciones.ddns.net:6034)

> Usuario: `telecomunicaciones`  
> Contraseña: `cohorte*2024`
---

# 📘 1 - Introducción

El presente proyecto tiene como finalidad diseñar e implementar un sistema IoT para el monitoreo y control de un cultivo hidropónico. El enfoque está basado en el Aprendizaje Basado en Proyectos (ABP) integrando conocimientos adquiridos en distintas materias técnicas de segundo año.

## 🧠 Justificación

- Automatización de cultivos.
- Acceso remoto a los datos.
- Toma de decisiones basada en analítica.

## 🎯 Objetivos

- Recolectar datos ambientales (humedad, pH, temperatura).
- Visualizar y analizar datos con dashboards.
- Aplicar control remoto mediante reglas automáticas.
- Documentar el desarrollo siguiendo buenas prácticas de software.
---

# ⚙️ 2 - Instalación del entorno

## 📦 Requisitos previos
- Docker Desktop o Docker Engine
- Git

## 🔁 Clonar el repositorio

git clone https://github.com/ISPC-PI-II-2024/Proyecto_hidroponico.git
cd Proyecto_hidroponico

### 📄 3 - Arquitectura
```markdown
# 🧭 3 - Arquitectura del sistema

## 🌐 Diagrama general

## 🔗 Descripción de componentes

- **ESP32**: Captura datos de sensores.
- **Mosquitto**: Transmite los datos por MQTT.
- **Node-RED**: Procesa y enruta los datos.
- **MariaDB**: Almacena registros históricos.
- **Grafana**: Visualiza tendencias y análisis.
---

# 🔄 4 - Automatización con Node-RED

Node-RED permite crear flujos lógicos para procesar, transformar y reenviar datos provenientes de sensores conectados vía MQTT.

## 🔧 Funcionalidades implementadas

- Recepción de datos desde ESP32 por MQTT.
- Filtrado y reenvío de datos a MariaDB.
- Reglas de control automático (p. ej., encendido de bombas).

## 🌐 Flujos visuales

(Sugerencia: insertar aquí una imagen del flujo Node-RED o exportar como JSON y documentarlo)

## 🔐 Seguridad

El acceso a Node-RED está protegido con usuario y contraseña.
---

# 📊 5 - Visualización con Grafana

Grafana permite construir dashboards con paneles que muestran en tiempo real o históricamente los datos del sistema hidropónico.

## 🧩 Paneles incluidos
- Temperatura del ambiente.
- Humedad del sustrato.
- Niveles de pH.

## 🔄 Fuentes de datos
- MariaDB integrada como data source.

## 🌐 Acceso
[http://telecomunicaciones.ddns.net:8080/grafana/](http://telecomunicaciones.ddns.net:8080/grafana/)
---

# 🗃️ 6 - Base de datos MariaDB

MariaDB almacena los datos recolectados desde los sensores y enviados por Node-RED.

## 🧱 Estructura
- `lecturas` (id, tipo_sensor, valor, timestamp)
- `alertas` (id, tipo, descripcion, timestamp)

## 🔌 Conexión
- Host: `telecomunicaciones.ddns.net`
- Puerto: `6034`
- Usuario: `telecomunicaciones`
- Contraseña: `cohorte*2024`

Acceso a través de gestores como MariaDB o MySQL Workbench.
---

# 📡 7 - Broker MQTT (Mosquitto)

El broker MQTT se usa para recibir datos desde sensores IoT.

## 📍 Datos de conexión
- Broker: `mqtt://telecomunicaciones.ddns.net`
- Puerto: `2480` (MQTT) / `9480` (WebSocket)
- Usuario: `telecomunicaciones`
- Contraseña: `cohorte*2024`

## 📫 Topics utilizados
- `hidroponico/temperatura`
- `hidroponico/humedad`
- `hidroponico/ph`

## 🧪 Pruebas
Podés usar MQTT Explorer para conectar y verificar los tópicos activos.
--- 

# 🌐 8 - API REST

Se desarrolló una API RESTful en Node.js utilizando Express para acceder a los datos del sistema.

## 📂 Ubicación en el repositorio
`/api`

## 🧪 Endpoints principales
| Método | Endpoint | Descripción |
|--------|----------|-------------|
| GET | `/api/lecturas` | Obtener todas las lecturas |
| GET | `/api/lecturas/:id` | Obtener una lectura específica |
| POST | `/api/lecturas` | Agregar una nueva lectura |
| DELETE | `/api/lecturas/:id` | Eliminar una lectura |

## 🔐 Seguridad
Actualmente sin autenticación, se recomienda añadir JWT o middleware de acceso en producción.

## 🧰 Tecnologías
- Node.js
- Express
- Body-parser
- Conexión a MariaDB
---

## 📄 Licencia

Distribuido bajo la Licencia Creative Commons.

---