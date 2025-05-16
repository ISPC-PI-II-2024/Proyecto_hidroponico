## Pruebas de lectura del sensor de flujo YF-S401

* Este código lee el sensor de flujo YF-S401 y calcula la tasa de flujo en litros por minuto.
* El sensor de flujo YF-S401 genera un pulso por cada 7.5 ml de agua que pasa a través de él.
* El código utiliza una interrupción para contar los pulsos generados por el sensor y calcula la tasa de flujo cada segundo.
* El resultado se imprime en el monitor serial.

* Simulación online en wokwi: https://wokwi.com/projects/430498455168999425