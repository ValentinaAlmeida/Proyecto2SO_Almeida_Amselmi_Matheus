#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include "queue.h"

extern Queue colaPrioridad0;
extern Queue colaPrioridad1;
extern Queue colaPrioridad2;

extern int ticActual;
extern bool simulacionViva;

void inicializarScheduler(void);
void* hilo_cpu(void* arg);
void calcular_metricas(PCB procesos[], int total_procesos);

#endif
