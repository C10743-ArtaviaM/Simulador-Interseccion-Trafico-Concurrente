/*
 * interseccion.c
 * Simulador de Interseccion de Trafico Concurrente
 *
 * CI-0117 Programacion Paralela & Concurrente - I Ciclo 2026
 *
 * Autores: Mauricio Artavia Monge
 * Daniel Rodriguez Ruiz
 */
#define _GNU_SOURCE

#include "interseccion.h"

#include <unistd.h>

#define _POSIX_C_SOURCE 200809L

/*
 * =============================================================================
 * VARIABLES GLOBALES
 * =============================================================================
 */
const char* NOMBRE_CARRIL[N_CARRILES] = {"Norte", "Sur", "Este", "Oeste"};
char vehiculo_en_cruce[16] = "";
int vehiculos_cruzados = 0;
int accidentes = 0;
int en_cruce = 0;
int cola[N_CARRILES] = {0};
int cruzados_por_carril[N_CARRILES] = {0};
sem_t semaforo_cruce;
sem_t mutex_contadores;

/*
 * =============================================================================
 * UTILITARIO
 * =============================================================================
 */
double tiempo_en_segundos(struct timespec inicio, struct timespec fin) {
  return (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
}

/*
 * =============================================================================
 * FASE 1 — sin sincronizacion
 * =============================================================================
 */
void* funcion_carril_fase1(void* arg) {
  args_carril_t* datos = (args_carril_t*)arg;
  int id = datos->id;
  unsigned int semilla = (unsigned int)id * 1234 + 1;

  for (int i = 1; i <= datos->n_vehiculos; i++) {
    /* 1. Generar vehiculo e incrementar cola */
    cola[id]++;

    /*
     * 2. Verificar en_cruce SIN proteccion - aqui esta la race condition
     *    Entre esta lectura y el en_cruce = 1 de abajo, otro hilo puede entrar
     *    - esa ventana de tiempo es el problema TOCTOU.
     */
    if (en_cruce == 1) {
      accidentes++;
      printf("[%s-%03d] entrando al cruce  <-  ACCIDENTE con %s\n",
             NOMBRE_CARRIL[id], i, vehiculo_en_cruce);
    } else {
      printf("[%s-%03d] entrando al cruce\n", NOMBRE_CARRIL[id], i);
    }

    /* 3. Marcar cruce ocupado y cruzar */
    en_cruce = 1;
    snprintf(vehiculo_en_cruce, sizeof(vehiculo_en_cruce), "%s-%03d",
             NOMBRE_CARRIL[id], i);
    usleep(2000 + rand_r(&semilla) % 3001); /* 2000 - 5000 us*/

    /* 4. Liberar cruce y actualizo contadores */
    en_cruce = 0;
    cruzados_por_carril[id]++;
    vehiculos_cruzados++;
    printf("[%s-%03d] cruce completado\n", NOMBRE_CARRIL[id], i);
  }

  return NULL;
}

/*
 * =============================================================================
 * FASE 2 - con semaforos
 * =============================================================================
 */
void* funcion_carril_fase2(void* arg) {
  args_carril_t* datos = (args_carril_t*)arg;
  int id = datos->id;
  unsigned int semilla = (unsigned int)id * 1234 + 1;

  for (int i = 1; i <= datos->n_vehiculos; i++) {
    /* 1. Generar vehiculo e implementar cola */
    cola[id]++;

    /*
     * 2. Intentamos entrar al cruce - sem_wait garantiza exclusion mutua. Solo
     * un hilo puede pasar este punto a la vez.
     */
    printf("[%s-%03d] sem_wait() -> cruzando\n", NOMBRE_CARRIL[id], i);
    sem_wait(&semaforo_cruce);

    /*
     * 3. Seccion critica: en_cruce, usleep y la liberacion bajo el mismo
     * semaforo. No hay ya ventana TOCTOU porque nadie mas puede entrar aqui.
     */
    en_cruce = 1;
    usleep(2000 + rand_r(&semilla) % 3001); /* 2000-5000 us */
    en_cruce = 0;

    sem_post(&semaforo_cruce);
    printf("[%s-%03d] sem_post() -> cruce libre\n", NOMBRE_CARRIL[id], i);

    /* 4. Actualizamos contadores protegidos por mutex_contadores */
    sem_wait(&mutex_contadores);
    vehiculos_cruzados++;
    cruzados_por_carril[id]++;
    sem_post(&mutex_contadores);

    printf("[%s-%03d] cruce completado\n", NOMBRE_CARRIL[id], i);
  }
  return NULL;
}

/*
 * =============================================================================
 * REPORTE FINAL
 * =============================================================================
 */
void imprimir_reporte(double tiempo_fase1, double tiempo_fase2) {
  /* TODO: implementar en Fase 3 */
  (void)tiempo_fase1;
  (void)tiempo_fase2;
}

/*
 * =============================================================================
 * MAIN
 * =============================================================================
 */
int main(void) {
  pthread_t hilos[N_CARRILES];
  args_carril_t args[N_CARRILES];
  struct timespec t_inicio, t_fin;
  double tiempo_fase1 = 0.0;
  double tiempo_fase2 = 0.0;

  printf("========================================================\n");
  printf("  SIMULADOR DE INTERSECCION DE TRAFICO - CI-0117\n");
  printf("  Carriles: %d | Vehiculos por carril: %d\n", N_CARRILES,
         N_VEHICULOS);
  printf("========================================================\n\n");

  /* =-=-= FASE 1 =-=-= */
  printf("--- FASE 1: Sin sincronizacion ---\n");

  vehiculos_cruzados = 0;
  accidentes = 0;
  en_cruce = 0;
  for (int i = 0; i < N_CARRILES; i++) {
    cola[i] = 0;
    cruzados_por_carril[i] = 0;
  }

  clock_gettime(CLOCK_MONOTONIC, &t_inicio);
  for (int i = 0; i < N_CARRILES; i++) {
    args[i].id = i;
    args[i].n_vehiculos = N_VEHICULOS;
    pthread_create(&hilos[i], NULL, funcion_carril_fase1, &args[i]);
  }
  for (int i = 0; i < N_CARRILES; i++) pthread_join(hilos[i], NULL);
  clock_gettime(CLOCK_MONOTONIC, &t_fin);
  tiempo_fase1 = tiempo_en_segundos(t_inicio, t_fin);

  printf("\n--- FASE 2: Con semaforos ---\n");

  vehiculos_cruzados = 0;
  accidentes = 0;
  en_cruce = 0;

  for (int i = 0; i < N_CARRILES; i++) {
    cola[i] = 0;
    cruzados_por_carril[i] = 0;
  }

  sem_init(&semaforo_cruce, 0, 1);
  sem_init(&mutex_contadores, 0, 1);

  clock_gettime(CLOCK_MONOTONIC, &t_inicio);

  for (int i = 0; i < N_CARRILES; i++) {
    args[i].id = i;
    args[i].n_vehiculos = N_VEHICULOS;

    pthread_create(&hilos[i], NULL, funcion_carril_fase2, &args[i]);
  }

  for (int i = 0; i < N_CARRILES; i++) {
    pthread_join(hilos[i], NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &t_fin);
  tiempo_fase2 = tiempo_en_segundos(t_inicio, t_fin);

  sem_destroy(&semaforo_cruce);
  sem_destroy(&mutex_contadores);

  /*=-=-= REPORTE =-=-=*/
  imprimir_reporte(tiempo_fase1, tiempo_fase2);

  return 0;
}
