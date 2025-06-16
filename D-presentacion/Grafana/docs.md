# 📊 5 - Visualización con Grafana

Grafana es la herramienta utilizada para visualizar los datos recolectados por los sensores del sistema hidropónico. A través de paneles dinámicos, permite monitorear variables críticas como temperatura, humedad, y pH, facilitando el análisis y la toma de decisiones.

---

## 🔌 Conexión con MariaDB

Grafana se integra directamente con la base de datos MariaDB, donde se almacenan las lecturas enviadas desde Node-RED.

### 🔐 Datos de conexión

- **Host:** `telecomunicaciones.ddns.net`
- **Puerto:** `6034`
- **Usuario:** `telecomunicaciones`
- **Contraseña:** `cohorte*2024`
- **Base de datos:** Nombre asignado en tu contenedor 

Configuración realizada desde:  
`Connections → Data Sources → Add new → MySQL (MariaDB)`

---

## 🧩 Paneles incluidos en el Dashboard "Hidroponia"

En el dashboard actual se visualizan:

- 🌡️ **Temperatura ambiente**
- 💧 **Humedad del sustrato**
- 🧪 **Nivel de pH**

Cada panel está basado en una consulta SQL hacia la tabla `lecturas`, filtrando por tipo de sensor.

---

## 🧠 Cómo editar y entender un panel

1. Accedé a: [Dashboard Hidroponia](http://telecomunicaciones.ddns.net:8080/grafana/d/4946fc4f-6bef-4771-b360-4d32d8c6951a/hidroponia?orgId=1)
2. En cada panel, hacé clic en los tres puntos (︙) > **Edit**.
3. Verás:
   - La **consulta SQL** al backend MariaDB
   - Tipo de visualización (línea, gauge, tabla)
   - Filtros aplicados (`$__timeFilter(timestamp)`)

### 📝 Ejemplo de consulta para temperatura

```sql
SELECT
  UNIX_TIMESTAMP(timestamp) as time_sec,
  valor as value
FROM lecturas
WHERE tipo_sensor = 'temperatura'
  AND $__timeFilter(timestamp)
ORDER BY timestamp ASC
