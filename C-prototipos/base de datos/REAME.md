# Proyecto Hidropónico - Base de Datos

Este proyecto contiene el diseño y estructura de una base de datos relacional para el sistema hidropónico inteligente. La base de datos permite registrar sensores, lecturas, actuadores, eventos de control y sistemas instalados, siendo compatible con una interfaz tipo **dashboard** como **Node-RED** o **Grafana** para el monitoreo y control en tiempo real.

---

## Estructura de la Base de Datos

### Tablas principales:

1. **sistemas**
   - Representa cada instalación física del sistema hidropónico.
   - Un sistema puede tener múltiples sensores y actuadores.

2. **sensores**
   - Representa los sensores físicos instalados (ej: temperatura, humedad, pH).
   - Cada sensor está vinculado a un sistema y tiene un tipo, modelo, pin de entrada, etc.

3. **lecturas**
   - Almacena cada lectura generada por un sensor en una fecha y hora determinada.
   - Está directamente relacionada con un sensor.

4. **actuadores**
   - Representa los dispositivos controlables (bombas, luces, ventiladores, etc.).
   - Cada actuador está conectado a un sistema y tiene un estado actual (`ON` o `OFF`).

5. **eventos_control**
   - Registra los eventos donde un actuador fue activado o desactivado.
   - Puede incluir el nombre de la persona que realizó la acción (campo opcional `nombre_usuario`), aunque no se requiere autenticación.

6. **umbrales** 
   - Define los valores mínimos y máximos aceptables para ciertos tipos de sensores.
   - Permite generar alertas cuando una lectura está fuera de rango.

---

## 🔗 Relaciones entre tablas

```plaintext
sistemas ───< sensores ───< lecturas
        └──< actuadores ───< eventos_control

## Su Relación

### 1. **Inicio del ciclo - Lecturas desde los sensores**

- Cada **sensor** está instalado en un sistema hidropónico y está registrado en la tabla `sensores`.
- Cuando un sensor mide un dato (ej: humedad, temperatura, pH), se genera una **lectura** que se guarda en la tabla `lecturas`, junto con la **fecha**, **hora** y el **valor medido**.

> Ejemplo: el sensor de humedad `H1` toma una lectura de 35% a las 10:00 AM. Esa lectura se guarda en `lecturas` con relación directa al sensor `H1`.

---

### 2. **Visualización de datos en el dashboard**

- El sistema (por ejemplo, Node-RED o Grafana) consulta las lecturas recientes de la tabla `lecturas` para mostrar gráficos y datos en tiempo real.
- Cada sensor tiene un `tipo` (temperatura, humedad, etc.), lo cual permite agrupar y visualizar los datos adecuadamente.

---

### 3. **Análisis de condiciones - Comparación con umbrales**

- Si existe un **umbral** definido para cierto tipo de sensor (en la tabla `umbrales`), el sistema compara la lectura actual con esos valores.
- Si el valor está fuera del rango definido, se genera una alerta o se activa una lógica de control automático.

> Ejemplo: si la temperatura supera los 35 °C y el umbral máximo es 30 °C, se activa automáticamente un ventilador.

---

### 4. **Activación de actuadores (control manual o automático)**

- El sistema puede activar un **actuador** (como una bomba, luz o ventilador), el cual está registrado en la tabla `actuadores`.
- Cada actuador tiene un campo `estado_actual` (`ON` o `OFF`).
- Cuando se activa o desactiva un actuador, se registra un **evento** en la tabla `eventos_control`, que incluye la fecha, hora y, opcionalmente, el `nombre_usuario` si fue proporcionado desde el dashboard.

> Ejemplo: la bomba `B1` se enciende a las 10:05 AM. El sistema actualiza `estado_actual = 'ON'` en la tabla `actuadores`, y guarda un registro en `eventos_control` indicando qué actuador fue activado, a qué hora, y por quién si se ingresó el nombre.

---

### 5. **Acceso libre para múltiples personas**

- Hasta 20 personas pueden operar el sistema desde los dashboards sin necesidad de registrarse ni autenticarse.
- El campo `nombre_usuario` en `eventos_control` es opcional y puede usarse para dejar un registro manual de quién realizó una acción.
- No existen restricciones de acceso a nivel de base de datos, por lo que todos los operadores tienen acceso completo.

> Ejemplo: cualquier persona del equipo puede encender o apagar un actuador desde Node-RED. Si desea dejar su nombre, puede ingresarlo desde la interfaz para que se registre en la base de datos.

