# Proyecto Hidropónico - Base de Datos Relacional (MySQL)

Este proyecto contiene el diseño y estructura de una base de datos relacional para el sistema hidropónico inteligente. La base de datos permite registrar sensores, lecturas, actuadores, acciones de usuarios y sistemas instalados, siendo compatible con una interfaz tipo **dashboard** para el monitoreo y control del sistema en tiempo real.

---

## Estructura de la Base de Datos

### Tablas principales:

1. **usuarios**
   - Registra los usuarios del sistema (técnicos, administradores).
   - Cada usuario puede tener uno o varios sistemas asociados.

2. **sistemas**
   - Representa cada instalación física del sistema hidropónico.
   - Un sistema puede tener múltiples sensores y actuadores.

3. **sensores**
   - Representa los sensores físicos instalados (ej: temperatura, humedad, pH).
   - Cada sensor está vinculado a un sistema y tiene un tipo, modelo, pin de entrada, etc.

4. **lecturas**
   - Almacena cada lectura generada por un sensor en una fecha y hora determinada.
   - Está directamente relacionada con un sensor.

5. **actuadores**
   - Representa los dispositivos controlables (bombas, luces, ventiladores, etc.).
   - Cada actuador está conectado a un sistema y tiene un estado actual (`ON` o `OFF`).

6. **eventos_control**
   - Registra los eventos donde un actuador fue activado o desactivado.
   - Asocia la acción con el usuario responsable y la hora exacta.

7. **umbrales** 
   - Define los valores mínimos y máximos aceptables para ciertos tipos de sensores.
   - Permite generar alertas cuando una lectura está fuera de rango.

---

## 🔗 Relaciones entre tablas

```plaintext
usuarios ─────┐
              └──< sistemas ───< sensores ───< lecturas
                           └──< actuadores ───< eventos_control
                                                     ↑
                                                usuarios


## Su Relacion

### 1. **Inicio del ciclo - Lecturas desde los sensores**

- Cada **sensor** está instalado en el sistema hidropónico y está vinculado a la tabla `sensores`.
- Cuando un sensor mide un dato (ej: humedad, temperatura, pH), se genera una **lectura** que se guarda en la tabla `lecturas`, junto con la **fecha**, **hora** y el **valor medido**.

> Ejemplo: el sensor de humedad `H1` toma una lectura de 35% a las 10:00 AM. Esa lectura se guarda en `lecturas` con una relación directa al sensor `H1`.

---

### 2. **Visualización de datos en el dashboard**

- El sistema consulta las lecturas recientes de la tabla `lecturas` para mostrar gráficos y datos en tiempo real en el dashboard.
- Cada sensor tiene un `tipo` (temperatura, humedad, etc.) que permite agrupar los datos y presentarlos correctamente.

---

### 3. **Análisis de condiciones - Comparación con umbrales**

- Si existe un **umbral** definido para cierto tipo de sensor (en la tabla `umbrales`), el sistema compara el valor actual con ese umbral.
- Si el valor excede los límites definidos, se activa una lógica de control automática o se genera una alerta.

> Ejemplo: si la temperatura supera los 35 °C y el umbral máximo es 30 °C, se activa automáticamente un ventilador.

---

### 4. **Activación de actuadores (control manual o automático)**

- El sistema puede activar un **actuador** (como una bomba, luz o ventilador), que está registrado en la tabla `actuadores`.
- Cada actuador tiene un campo `estado_actual` (`ON` o `OFF`).
- Cuando se activa o desactiva un actuador, se registra un **evento** en la tabla `eventos_control`.

> Ejemplo: la bomba `B1` se enciende a las 10:05 AM. El sistema actualiza `estado_actual = 'ON'` en la tabla `actuadores`, y guarda un registro en `eventos_control` indicando quién lo activó, cuándo y a qué actuador.

---

### 5. **Usuarios y sistemas**

- Cada `usuario` (por ejemplo, un técnico o administrador) está registrado en la tabla `usuarios`.
- Un usuario puede tener uno o varios **sistemas hidropónicos** asociados (tabla `sistemas`).
- Esto permite que cada usuario visualice y controle solo sus sistemas en el dashboard.

> Ejemplo: El usuario "X alumno" tiene dos sistemas en dos invernaderos. Desde su cuenta en el dashboard puede ver los sensores, actuadores y lecturas de ambos.S

---
