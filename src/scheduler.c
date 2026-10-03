#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
//Cola de prioridad 0,1,2
Queue colaPrioridad0;
Queue colaPrioridad1;
Queue colaPrioridad2;

int ticActual= 0;//contador de tics que leva la simulacion
bool simulacionViva= true;//e este momento el hilo de el CPU trabaja

void inicializarScheduler() {
    inicializarQueue(&colaPrioridad0);//incializamos la cola de prioridad ala 
    inicializarQueue(&colaPrioridad1);//inicializamos la cola de prioridad media 
    inicializarQueue(&colaPrioridad2);//inicializamos la cola de prioridad baja 
}//fin de la funcion

void* hilo_cpu(void* arg) {
PCB* centinelaCPU = NULL;//quien esta en el CPU
int colaOrigen = -1;//de que cola viene mi centinela 
if (centinelaCPU != NULL && colaOrigen > 0 && !isEmpty(&colaPrioridad0)) {
            centinelaCPU->estado = LISTO;
            if (colaOrigen == 1) {
                colocarfrente(&colaPrioridad1, centinelaCPU);
            } else if (colaOrigen == 2) {
                colocarfrente(&colaPrioridad2, centinelaCPU);
            }
            centinelaCPU = NULL;
            colaOrigen = -1;
        }

        if (centinelaCPU == NULL) {
            if (!isEmpty(&colaPrioridad0)) {[cite: 1]
                centinelaCPU = dequeue(&colaPrioridad0);
                colaOrigen = 0;
            } else if (!isEmpty(&colaPrioridad1)) {[cite: 1]
                centinelaCPU = dequeue(&colaPrioridad1);
                colaOrigen = 1;
            } else if (!isEmpty(&colaPrioridad2)) {[cite: 1]
                centinelaCPU = dequeue(&colaPrioridad2);
                colaOrigen = 2;
            }
        }
        if (centinelaCPU != NULL) {
            centinelaCPU->estado = EJECUCION;

            centinelaCPU->tiempo_restante--;
            centinelaCPU->quantum_consumido++;

            if (centinelaCPU->tiempo_restante == 0) {
                centinelaCPU->estado = TERMINADO;
                centinelaCPU->tiempo_retorno = ticActual + 1;
                
                centinelaCPU = NULL;
                colaOrigen = -1;
            }
            else if (colaOrigen == 0 && centinelaCPU->quantum_consumido == 2) {[cite: 1]
                centinelaCPU->quantum_consumido = 0;
                centinelaCPU->estado = LISTO;
                enqueue(&colaPrioridad0, centinelaCPU);
                
                centinelaCPU = NULL;
                colaOrigen = -1;
            } 
            else if (colaOrigen == 1 && centinelaCPU->quantum_consumido == 4) {[cite: 1]
                centinelaCPU->quantum_consumido = 0;
                centinelaCPU->estado = LISTO;
                enqueue(&colaPrioridad1, centinelaCPU);
                
                centinelaCPU = NULL;
                colaOrigen = -1;
            }
        }

        usleep(100000);
        ticActual++;
        return NULL;
    }

void calcular_metricas(PCB procesos[], int total_procesos) {
    if (total_procesos <= 0) return;

    double suma_espera = 0.0;
    double suma_retorno = 0.0;

    printf("\n=== METRICAS DE RENDIMIENTO DEL SISTEMA ===\n");
    printf("ID\tLlegada\tBurst\tFin\tTurnaround\tEsperas\n");

    for (int i = 0; i < total_procesos; i++) {
        int turnaround = procesos[i].tiempo_retorno - procesos[i].tiempo_llegada;
        int espera = turnaround - procesos[i].tiempo_rafaga;

        if (espera < 0) espera = 0;

        suma_retorno += turnaround;
        suma_espera += espera;

        printf("%d\t%d\t%d\t%d\t%d\t\t%d\n",
               procesos[i].id,
               procesos[i].tiempo_llegada,
               procesos[i].tiempo_rafaga,
               procesos[i].tiempo_retorno,
               turnaround,
               espera);
    }

    double promedio_retorno = suma_retorno / total_procesos;
    double promedio_espera = suma_espera / total_procesos;
    double throughput = (double)total_procesos / (tic_actual > 0 ? tic_actual : 1);

    printf("-------------------------------------------\n");
    printf("Tiempo Medio de Retorno (Turnaround): %.2f tics\n", promedio_retorno);
    printf("Tiempo Medio de Espera (Waiting Time): %.2f tics\n", promedio_espera);
    printf("Rendimiento del Sistema (Throughput):  %.4f proc/tic\n", throughput);
    printf("===========================================\n\n");
}
