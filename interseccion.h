#ifndef INTERSECCION_H
#define INTERSECCION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

/*
 * =============================================================================
 * CONSTANTES
 * =============================================================================
 */
#define N_CARRILES 4
#define N_VEHICULOS 10

/* Identificadores de carril */
#define NORTE 0
#define SUR 1
#define ESTE 2
#define OESTE 3

extern const char *NOMBRE_CARRIL[N_CARRILES];

/*
 * =============================================================================
 * ESTRUCTURAS
 * =============================================================================
 */
typedef struct {
    int id; /* 0 = Norte, 1 = Sur, 2 = Este, 3 = Oeste */
    int n_vehiculos; /* Cuantos vehiculos debe generar */
} args_carril_t;

/*
 * =============================================================================
 * VARIABLES GLOBALES
 * =============================================================================
 */
extern int vehiculos_cruzados;
extern int accidentes;
extern int en_cruce;
extern int cola[N_CARRILES];
extern int cruzados_por_carril[N_CARRILES];
extern sem_t semaforo_cruce;
extern sem_t mutex_contadores;

/*
 * =============================================================================
 * PROTOTIPOS
 * =============================================================================
 */
void *funcion_carril(void *arg);
void imprimir_reporte(double tiempo);
double tiempo_en_segundos(struct timespec inicio, struct timespec fin);

#endif /* INTERSECCION_H */
