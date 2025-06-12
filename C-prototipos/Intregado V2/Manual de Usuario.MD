

# 📝 **Manual de Usuario: Sistema Hidropónico Inteligente en ESP32**  

## 📌 **1. Introducción**  
Este sistema automatiza el monitoreo y control de cultivos hidropónicos mediante sensores de temperatura, humedad, pH, nivel de agua y luz. Su interfaz gráfica permite configurar parámetros y supervisar el estado del cultivo desde una pantalla TFT.  

## 📌 **2. Encendido del Sistema**  
1. Conecta el ESP32 a una fuente de alimentación.  
2. La pantalla mostrará el mensaje **"Iniciando..."** mientras se inicializan los módulos.  
3. Luego, se mostrará el **menú principal**, listo para la navegación.  

## 📌 **3. Menú Principal**  
El usuario puede navegar entre opciones utilizando **tres botones físicos**:  
✔ Botón **Izquierda**: Mueve la selección hacia arriba.  
✔ Botón **Derecha**: Mueve la selección hacia abajo.  
✔ Botón **Confirmar**: Ingresa a la opción seleccionada.  

Las opciones disponibles son:  
✅ **Monitoreo de sensores** – Muestra temperatura, humedad, pH y flujo de agua en tiempo real.  
✅ **Recetas de cultivo** – Permite seleccionar parámetros óptimos para distintos cultivos.  
✅ **Configuración WiFi** – Escanea redes disponibles y permite conectarse a una red nueva.  
✅ **Estado del sistema** – Indica si la bomba, luces y sensores están activos.  

## 📌 **4. Uso de las Recetas de Cultivo**  
Desde el menú de **Recetas**, el usuario puede:  
✔ **Seleccionar** una receta de cultivo para ajustar parámetros automáticamente.  
✔ **Editar** una receta y modificar sus valores (temperatura, pH, humedad, iluminación).  
✔ **Guardar** cambios para aplicarlos de inmediato.  

## 📌 **5. Conexión WiFi y MQTT**  
1. Accede a **Configuración WiFi** en el menú.  
2. Se mostrará una lista de redes disponibles.  
3. Usa los botones para seleccionar la red deseada.  
4. Ingresa la contraseña y confirma la conexión.  
5. Si la conexión es exitosa, el sistema se sincroniza con el servidor MQTT y comienza a enviar datos.  

## 📌 **6. Modo Automático vs. Manual**  
✔ **Modo Automático:** El sistema ajusta riego, iluminación y alarmas según las mediciones de los sensores.  
✔ **Modo Manual:** El usuario activa/desactiva la bomba y las luces desde el menú.  

## 📌 **7. Manejo de Alertas y Seguridad**  
Si algún sensor detecta condiciones fuera de los parámetros seguros (por ejemplo, agua insuficiente o pH extremo), el sistema:  
🚨 Muestra un aviso visual en pantalla.  
📡 Envía una alerta vía MQTT o LoRa a otros dispositivos conectados.  
💡 Recomienda una acción para corregir el problema.  

---