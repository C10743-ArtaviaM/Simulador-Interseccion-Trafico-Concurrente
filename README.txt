Instrucciones de compilación: 
Para poder correr el programa desde terminal se utiliza el comando "make all" para compilar el archivo, a su vez, para ejecutarlo se ejecuta el comando "make run"

a) Race Condition: Identifique exactamente cuáles líneas del código de Fase 1 constituyen una race condition. ¿Por qué la instrucción en_cruce = 1 sola no es suficiente para garantizar exclusión mutua?

R1/ La race condition se da justamente cuando dos vehiculos intentan cruzar al mismo tiempo en la fase 1, específicamente en las líneas donde se hace:
if (en_cruce == 1) {
      accidentes++;
}
   
en_cruce = 1;
en_cruce = 0;

Ya que esa es una variable global la cual se está modificando sin ningún control para evitar condiciones de carrera.
Esa instrucción no es suficiente porque los hilos al poder acceder a la variable global al mismo momento pueden entrar y modificar el valor de la variable global en_cruce al mismo instante, es por esto que es muy importante utilizar 
semáforo para evitar este problema.


b) Invariante del semáforo: ¿Qué invariante garantiza el semáforo binario en Fase 2?
Explíquelo con sus propias palabras. ¿Por qué se inicializa en 1 y no en 0?

R2/ Garantiza que dos vehículos no intenten cruzar en el mismo momento, previniendo choques en la autopista. Se inicializa en 1 porque esto permite que el primer hilo entre de forma correcta, si se inicializara en 0 significa que el primer hilo no podría entrar, por lo que estaría bloqueado y se generaría un deadlock.
	
c) Overhead: ¿Por qué la Fase 2 suele tomar más tiempo que la Fase 1? ¿Este
overhead es aceptable? ¿En qué tipo de sistema NO sería aceptable?

R3/ Porque el proceso de gestionar los hilos requiere tiempo, el sistema se encarga de que únicamente un hilo pueda modificar la variable local, conviertiendo esta parte del código en una versión serial, mientras que en la fase 1 como los hilos pueden modificar esta variable en cualquier momento el programa no toma ese tiempo extra que en la fase 2 sí se toma. 
Sí es aceptable, ya que ese tiempo "extra" que tarda es clave para proteger las variables globales y evitar condiciones de carrera en los procesos.
No sería aceptable en sistemas donde el tiempo sea crítico, como por ejemplo sistemas médicos en el que unos segundos pueden significar la vida o la muerte de una persona o videojuegos en tiempo real.


d) Experimento: Presente los resultados del experimento de escala (N_VEHICULOS = 5, 20, 50). ¿Cómo cambia la tasa de accidentes al aumentar el número de vehículos?
¿Tiene sentido ese comportamiento?

N_VEHICULOS = 5
======== REPORTE FINAL ========
Fase 1 (sin sincronizacion):
  Total vehiculos:    20
  Accidentes:         3
  Vehiculos/carril:   Norte=5 Sur=5 Este=5 Oeste=5
  Tiempo simulacion: 0.022 segundos
Fase 2 (con semaforos):
  Total vehiculos:    20
  Accidentes:         0
  Vehiculos/carril:   Norte=5 Sur=5 Este=5 Oeste=5
  Tiempo simulacion: 0.073 segundos
ANALISIS:
  Overhead de sincronizacion: +0.050 seg (+226.2%)
========================================================

N_VEHICULOS = 20
======== REPORTE FINAL ========
Fase 1 (sin sincronizacion):
  Total vehiculos:    80
  Accidentes:         5
  Vehiculos/carril:   Norte=20 Sur=20 Este=20 Oeste=20
  Tiempo simulacion: 0.077 segundos
Fase 2 (con semaforos):
  Total vehiculos:    80
  Accidentes:         0
  Vehiculos/carril:   Norte=20 Sur=20 Este=20 Oeste=20
  Tiempo simulacion: 0.295 segundos
ANALISIS:
  Overhead de sincronizacion: +0.218 seg (+282.0%)
========================================================

N_VEHICULOS = 50
======== REPORTE FINAL ========
Fase 1 (sin sincronizacion):
  Total vehiculos:    200
  Accidentes:         6
  Vehiculos/carril:   Norte=50 Sur=50 Este=50 Oeste=50
  Tiempo simulacion: 0.187 segundos
Fase 2 (con semaforos):
  Total vehiculos:    200
  Accidentes:         0
  Vehiculos/carril:   Norte=50 Sur=50 Este=50 Oeste=50
  Tiempo simulacion: 0.725 segundos
ANALISIS:
  Overhead de sincronizacion: +0.538 seg (+288.3%)
========================================================

R4/ Conforme hay más vehiculos, se nota un crecimiento en la cantidad de accidentes, no obstante, la tasa de accidentes disminuye porcentualmente. Esto se da porque al principio todos los hilos compiten por el mismo recurso, o sea, cruzar la calle. Es aquí donde principalemente se dan los choques, este comportamiento sí tiene sentido en el contexto de semáforos e hilos.

e) Extensión (opcional, bonus +0.5): ¿Cómo modificaría el sistema para permitir que más de un vehículo cruce simultáneamente (carril de doble vía)? ¿Qué tipo de
semáforo usaría?
R5/ Un semáforo que permita cruzar más de un vehículo a la vez, ahora mismo se emplea un semáforo binario, por lo que sólo puede pasar uno, sin embargo, si se quisiera lograr que pasen dos se necesitaria un semáforo de conteo que permita pasar si son vehiculos que van hacia la misma dirección, esto válida que no choquen porque no se están interceptando al ir en la misma dirección. Si son vehiculos con distintas direcciones el semaforo solamente dejaría pasar de uno en uno para asegurarse que no choquen. 
