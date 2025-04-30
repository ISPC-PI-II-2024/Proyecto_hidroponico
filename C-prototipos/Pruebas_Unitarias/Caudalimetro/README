## Caudalímetro de efecto Hall YF-S201 o YF-S401
### ¿Qué es un caudalímetro?
Un caudalímetro es un sensor que mide el caudal de un líquido, o sea, cuánto fluido pasa por él por unidad de tiempo. Se expresa generalmente en litros por minuto (L/min).

### ¿Cómo funciona un caudalímetro de efecto Hall?
1. **Turbina interna:** Dentro del caudalímetro hay una pequeña hélice o turbina que gira cuando el agua fluye a través del sensor.

2. **Sensor de efecto Hall:** Al lado de la hélice hay un imán y un sensor Hall. Cada vez que el imán pasa frente al sensor, se genera un pulso digital.

3. **Salida digital:** El caudalímetro emite una señal cuadrada (ON/OFF) en un pin de salida (normalmente 5V o 3.3V). Esta señal se conecta a un pin digital de tu microcontrolador.

4. **Medición:** El microcontrolador cuenta cuántos pulsos llegan por segundo, y a partir de eso calcula el caudal.

### Ejemplo
Supongamos que un modelo te da esta fórmula:
```
Caudal (L/min) = (Pulsos por segundo) / 7.5
```

Entonces, si en un segundo contás 75 pulsos, el caudal sería:
```
75 / 7.5 = 10 L/min
```

### Proyecto en Wokwi
https://wokwi.com/projects/429039351948338177