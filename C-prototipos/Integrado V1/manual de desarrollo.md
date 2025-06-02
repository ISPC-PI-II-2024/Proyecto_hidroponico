## Manual de Desarrollo

### Descripcion
Este programa funcionara como la primer version del modulo integrado para el proyecto del sistema hidroponico, buscando compilar en este todas las pruebas unitarias realizadas previamente por el equipo, y buscando integrar lo realizado en un programa que haga uso de ello de la manera mas eficiente posible, dentro de nuestras capacidades.
Buscaremos mantener un directorio organizado con partes definidas que siendo funcionales, la falta de una no genere errores fatales al momento de la compilacion del mismo


### Estrcutura del Directorio:

- 📦Integrado V1
-  ┣ 📂.pio                                         || Integramos un comando en el .ini para que aqui se guarden los binarios
-  ┃ ┣ 📂build
-  ┃ ┃ ┣ 📂esp32dev
-  ┃ ┗ 📂libdeps
-  ┣ 📂.vscode
-  ┃ ┣ 📜c_cpp_properties.json
-  ┃ ┣ 📜extensions.json
-  ┃ ┗ 📜launch.json
-  ┣ 📂include
-  ┃ ┣ 📜configuracion.h                            || El archivo que gestionara las configuraciones globales
-  ┃ ┗ 📜README
-  ┣ 📂lib
-  ┃ ┗ 📜README
-  ┣ 📂sensor                                       || Se utilizada cada prueba unitaria previa como una libreria
-  ┃ ┣ 📂include                                    || Aca colocamos el archivo .h qu luego es llamado en main
-  ┃ ┣ 📂src                                        || Y aca al .cpp con las funciones y declaraciones definidas para su funcionamiento
-  ┃ ┗ 📜library.json                               || pequeño json que describe la libreria, tags, creadores, etc
-  ┣ 📂src
-  ┃ ┗ 📜main.cpp                                   || Elemento que llamara y coordinara el resto de los elemento
-  ┣ 📂test
-  ┃ ┗ 📜README                                     || Pruebas unitarias del sistema
-  ┣ 📜.gitignore
-  ┣ 📜manual de desarrollo.md                      || Documentacion del programa
-  ┗ 📜platformio.ini                               || inciador del programa. Registra ls librerias utilizadas  

------

### lib
Aqui se crea una carpeta para cada sensor/actuador/elemento.
Se le coloca el nombre, y se copian desde el directorio de pruebas unitarias el 
- .cpp en **src** de la carpeta 
- .h en **include**
- el manual de uso del sensor fuera del src
- Modificar el json con las caractesiticas requeridas

- **EJEMPLO**
  -  📦Buzzer
  -  ┣ 📂include
  -  ┃ ┗ 📜buzzer.h
  -  ┣ 📂src
  -  ┃ ┗ 📜buzzer.cpp
  -  ┣ 📜library.json
  -  ┗ 📜README

------

### Orquestacion
Vamos a crear diferentes elementos que nos permitan la orquestacion del sistema
 - conecciones_manager: Este articulara y gestionara las funciones relacionadas a wifi y gsm. Esto incluye gestion de credenciales, pinuot en caso de falla del gsm, reconecciones y logs de error
 - sensores_manager: articulara las funciones, pinouts, variales y devoluciones de los sensores
 - actuadores_manager: articulara las acciones activas como la bomba, luces y alarmas. Se encargada de adminstrar la respuesta por umbrales, y errores relacionados
 - configuracion.h: configuraciones globales del sistema
 - main.cpp buscaremso mantenerlo reducido, llamando a los manager que se encargaran de ejecutar internamente las orquestaciones de sus respectivos elementos a fin de mantener un procedimiento claro y consiso, facil de leer y dicernir posibles errores