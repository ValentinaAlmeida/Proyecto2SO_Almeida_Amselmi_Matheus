#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include "queue.h"

// Las 3 colas de prioridad del sistema
extern Queue cola_prioridad0; // RR Q=2 (Comandos)
extern Queue cola_prioridad1; // RR Q=4 (Perforadores)
extern Queue cola_prioridad2; // FCFS (Desgaste)

// Variable global de control del reloj (manejada por el Hilo Maestro)
extern int tic_actual;
extern bool simulacion_activa;

// Prototipos
void inicializar_planificador(void);
void* hilo_cpu(void* arg);
void calcular_metricas(PCB procesos[], int total_procesos);

#endif