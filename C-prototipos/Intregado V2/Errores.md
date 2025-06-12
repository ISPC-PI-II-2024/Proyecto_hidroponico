# Detalle de Errores y mensajes de Alarma

### Errores: 

| Código | Nombre                         | Mensaje                                           |
| ------ | ------------------------------ | ------------------------------------------------- |
| 0      | NONE                           | OK                                                |
| 1      | WIFI_INIT_FAIL                 | Error al inicializar WiFi                         |
| 2      | WIFI_AVAILABLE_NW_FAIL         | Error al escanear redes WiFi                      |
| 3      | WIFI_CONNECT_FAIL              | Error al conectar a la red WiFi                   |
| 4      | WIFI_CRED_SAVE_FAIL            | Error al guardar credenciales WiFi                |
| 5      | LORA_INIT_FAIL                 | Error al inicializar LoRa Mesh                    |
| 6      | LORA_SEND_FAIL                 | Error al enviar datos por LoRa                    |
| 7      | LORA_GET_FAIL                  | Error al recibir datos por LoRa                   |
| 8      | GSM_INIT_FAIL                  | Error al inicializar módulo GSM                   |
| 9      | GSM_LOOP_FAIL                  | Error en bucle GSM                                |
| 10     | GSM_STATE_FAIL                 | Estado GSM inválido                               |
| 11     | GSM_MQTTClient_FAIL            | Error al obtener cliente MQTT sobre GSM           |
| 12     | BLE_INIT_FAIL                  | Error al inicializar BLE Mesh                     |
| 13     | BLE_SEND_FAIL                  | Error al enviar datos por BLE                     |
| 14     | BLE_GET_FAIL                   | Error al recibir datos por BLE                    |
| 15     | MQTT_INIT_FAIL                 | Error al inicializar conexión MQTT                |
| 16     | MQTT_SEND_FAIL                 | Error al enviar datos por MQTT                    |
| 17     | RMENU_INIT_FAIL                | Error al inicializar menú de Recetas              |
| 18     | RMENU_MANAGE_FAIL              | Error al gestionar selección de receta            |
| 19     | WMENU_INIT_FAIL                | Error al inicializar menú de WiFi                 |
| 20     | WMENU_MANAGE_FAIL              | Error al gestionar selección de WiFi              |
| 21     | PMENU_INIT_FAIL                | Error al inicializar menú Principal               |
| 22     | PMENU_MANAGE_FAIL              | Error al procesar entrada del menú                |
| 23     | PMENU_SHOW_MSG_FAIL            | Error al mostrar mensaje en menú Principal        |
| 24     | SENS_INIT_FAIL                 | Error al inicializar sensores                     |
| 25     | SENS_TEMP_AGUA_FAIL            | Error al leer temperatura del agua                |
| 26     | SENS_DTH_FAIL                  | Error al leer sensor DHT                          |
| 27     | SENS_NIVEL_AGUA_FAIL           | Error al leer nivel de agua                       |
| 28     | SENS_FLUJO_AGUA_FAIL           | Error al calcular flujo de agua                   |
| 29     | SENS_LUZ_FAIL                  | Error al leer intensidad de luz                   |
| 30     | SENS_GAS_FAIL                  | Error al leer sensor de gas                       |
| 31     | SENS_CORRIENTE_FAIL            | Error al leer sensor de corriente                 |
| 32     | SENS_PH_FAIL                   | Error al leer sensor de pH                        |
| 33     | CONTROL_INIT_FAIL              | Error al inicializar módulo de Control            |
| 34     | CONTROL_ACT_BOMBA_FAIL         | Error al activar bomba                            |
| 35     | CONTROL_ACT_LUCES_FAIL         | Error al activar iluminación                      |
| 36     | CONTROL_VERIF_ALARM_FAIL       | Error al verificar alarmas                        |
| 37     | CONTROL_NOTIF_ESTADO_FAIL      | Error al notificar estado                         |
| 38     | CONTROL_AUTO_FAIL              | Error en rutina de control automático             |
| 39     | DISPLAY_INIT_FAIL              | Error al inicializar pantalla                     |
| 40     | DISPLAY_UPDATE_FAIL            | Error al actualizar pantalla                      |
| 41     | DISPLAY_SHOW_MSG_FAIL          | Error al mostrar mensaje en pantalla               |
| 42     | DISPLAY_SHOW_DATA_FAIL         | Error al mostrar datos en pantalla                 |
| 43     | DISPLAY_SHOW_STANDBY_FAIL      | Error al mostrar modo standby                      |
| 44     | DISPLAY_SHOW_ALERTA_FAIL       | Error al mostrar alerta de agua                    |
| 45     | DISPLAY_BOTONES_FAIL           | Error al manejar botones de la pantalla            |
| 46     | DISPLAY_DASHBOARD_FAIL         | Error al inicializar dashboard en pantalla         |
| 47     | RECEPTOR_INIT_FAIL             | Error al inicializar receptor                      |
| 48     | RECEPTOR_PROCESAR_FAIL         | Error al procesar datos del receptor               |
| 49     | RTC_INIT_FAIL                  | Error al inicializar RTC                           |
| 50     | RTC_GET_HORA_FAIL              | Error al obtener hora del RTC                      |
| 51     | RTC_SET_HORA_FAIL              | Error al sincronizar hora del RTC                  |
| 52     | TRANSMISOR_INIT_FAIL           | Error al inicializar transmisor                    |
| 53     | TRANSMISOR_ENVIAR_FAIL         | Error al enviar datos con el transmisor            |
| 54     | RECETAS_CARGAR_FAIL            | Error al cargar recetas                            |
| 55     | RECETAS_GUARDAR_FAIL           | Error al guardar recetas                           |
| 57     | RECETAS_SELECCIONAR_FAIL       | Error al seleccionar receta                        |
| 58     | RECETAS_MOSTRAR_FAIL           | Error al mostrar receta actual                     |
