#ifndef IMPRESION_H
#define IMPRESION_H

#include "PCB.h"

/**
 * @brief Tipos de eventos individuales que ocurren en la simulación.
 */
typedef enum {
    TIEMPO_LLEGADA,         // [T-XXXX] ARRIVAL: Sentinel ID: %d [Type: %s]
    QUANTUM_EXPIRADO,// [T-XXXX] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=%d)
    EXPROPIECE,   // [T-XXXX] PREEMPTION: Sentinel %d preempted by higher priority task
    FINALIZADO       // [T-XXXX] COMPLETED: Sentinel ID: %d -> Turnaround Time: %d ticks
} Evento;

//Esta es para imprimir los individuales, recibe el tic del momento exacto, el tipo de evento según mi enum, el PCB y quantum del nivel y adicionales
void impresionIndividual(int tic, Evento evento, const PCB *p, int parametroExtra);
//Esta imprime todo de golpe, osea todos los procesos, los tics completos y todo como en el ejemplo
void impresionCompleta(const PCB procesos[], int total_procesos, int total);

#endif 