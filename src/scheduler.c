#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "scheduler.h"
#include "impresion.h"

// Cola de prioridad 0,1,2
Queue colaPrioridad0;//prioridad alta
Queue colaPrioridad1;//prioridad media
Queue colaPrioridad2;//prioridad baja

int ticActual= 0;//contador de tics que lleva la simulacion
bool simulacionViva= true;//en este momento el hilo del CPU trabaja

// Funcion para inicializar las colas a usar
void inicializarScheduler() {//inicializamos las colas
    inicializarQueue(&colaPrioridad0);//inicializamos la cola de prioridad alta
    inicializarQueue(&colaPrioridad1);//inicializamos la cola de prioridad media
    inicializarQueue(&colaPrioridad2);//inicializamos la cola de prioridad baja
}//fin de la funcion

// Funcion auxiliar  para que funcione el hilo de Corina, se hizo para que esta planificacion se ejecute una vez cada que lo llemen 
int scheduler_tick(int tic, int verbose) {
    (void)tic;
    (void)verbose;
    return 0;
}

static PCB* terminadoPendiente = NULL;

void imprimirFinalizacionPendiente(void) {
    if (terminadoPendiente != NULL) {
        impresionIndividual(terminadoPendiente->tiempo_retorno, FINALIZADO, terminadoPendiente, 0);
        terminadoPendiente = NULL;
    }
}


// Funcion que aplica en round robin y planifica que centinela toma su puesto
void* hilo_cpu(void* arg) {//hilo del cpu que movera los hilos y vera las colas segun la prioridad
    (void)arg;
    imprimirFinalizacionPendiente();
    static PCB* centinelaCPU = NULL;//quien esta en el CPU
    static int colaOrigen = -1;//de que cola viene mi centinela
//esto es para aplicar la palnificacion con desalojo, si llega alquien de la cola 0(ma alta prioridad),quito al que 
    if (centinelaCPU != NULL && colaOrigen > 0 && !isEmpty(&colaPrioridad0)) {//si actualmente tenemos un centinela 
            centinelaCPU->estado = READY;//el centinela sera el elegido
            impresionIndividual(ticActual, EXPROPIECE, centinelaCPU, 0);
            if (colaOrigen == 1) {//reviso si el centinela elegido es de cola 1
                colocarfrente(&colaPrioridad1, centinelaCPU);//lo coloco al frente porque llego otro de mas alta pri
            } else if (colaOrigen == 2) {//si actualmente se esta ejecutando uno de la cola 2
                colocarfrente(&colaPrioridad2, centinelaCPU);//lo  coloco al fente de la cola porque llego uno de ma
            }
                centinelaCPU = NULL;//libero la CPU
                colaOrigen = -1;//no hay nadie ahora
    }

        if (centinelaCPU == NULL) {//si la CPU esta libre
            if (!isEmpty(&colaPrioridad0)) {//si la cola de prioridades altas no esta vacia
                centinelaCPU = dequeue(&colaPrioridad0);//saco y ateindo primero a estos de alata prioridad
                colaOrigen = 0;//indico de donde viene
            } else if (!isEmpty(&colaPrioridad1)) {//una vez que la cola de alta prioridad se queda vacia,revisamos 
                centinelaCPU = dequeue(&colaPrioridad1);//el elegido sera aquel que ha estado esperando en la cola
                colaOrigen = 1;//indico de donde viene
            } else if (!isEmpty(&colaPrioridad2)) {//una vez vacias las anteriores y esta de prioridades bajas no lo
                centinelaCPU = dequeue(&colaPrioridad2);//saco uno de la cola
                colaOrigen = 2;//indico de donde viene
            }
        }
        if (centinelaCPU != NULL) {//si la cpu no esta libre y tiene a un centinela
            centinelaCPU->estado = RUNNING;//actualizo su estado en el PCB
//a apartir de aqui, reviso los posibles ecenarios que le pueden pasar:

            //si termino el quantum de cola 0
            if (colaOrigen == 0 && centinelaCPU->quantum_consumido >= 2) {
                centinelaCPU->quantum_consumido = 0;//receteo el quantum porque ya lo cumplio y en la proxima iterac
                centinelaCPU->estado = READY;//listo, vuelve a la cola a esperar su turno
                bool hayOtro = !isEmpty(&colaPrioridad0);
                enqueue(&colaPrioridad0, centinelaCPU);//esta en la cola al final esperando su turno
                if (hayOtro) impresionIndividual(ticActual, QUANTUM_EXPIRADO, centinelaCPU, 2);

                centinelaCPU = NULL;//le doy chance a otro
                colaOrigen = -1;//todavia no pertenece a ninguna cola

                // Tomamos inmediatamente al recien llegado de la cola 0 (como el proceso 102)
                if (!isEmpty(&colaPrioridad0)) {
                    centinelaCPU = dequeue(&colaPrioridad0);
                    colaOrigen = 0;
                    centinelaCPU->estado = RUNNING;
                }
            }
            //si termino el quantum de cola 1
            else if (colaOrigen == 1 && centinelaCPU->quantum_consumido >= 4) {
                centinelaCPU->quantum_consumido = 0;//receteo el quantum porque ya lo cumplio y en la proxima iterac
                centinelaCPU->estado = READY;//listo, vuelve a la cola a esperar su turno
                bool hayOtro = !isEmpty(&colaPrioridad1);
                enqueue(&colaPrioridad1, centinelaCPU);//esta en la cola al final esperando su turno
                if (hayOtro) impresionIndividual(ticActual, QUANTUM_EXPIRADO, centinelaCPU, 4);

                centinelaCPU = NULL;//le doy chance a otro
                colaOrigen = -1;//todavia no pertenece a ninguna cola

                if (!isEmpty(&colaPrioridad0)) {
                    centinelaCPU = dequeue(&colaPrioridad0);
                    colaOrigen = 0;
                    centinelaCPU->estado = RUNNING;
                } else if (!isEmpty(&colaPrioridad1)) {
                    centinelaCPU = dequeue(&colaPrioridad1);
                    colaOrigen = 1;
                    centinelaCPU->estado = RUNNING;
                }
            }

            if (centinelaCPU != NULL) {
                centinelaCPU->tiempo_restante--;//le quito su tiempo restante porque ya se esta ejecutando disminuye su 
                centinelaCPU->quantum_consumido++;//y ya consumio un quantum de su tiempo

                if (centinelaCPU->tiempo_restante == 0) {//si ya termino
                    centinelaCPU->estado = FINISHED;//actualizo su estado
                    centinelaCPU->tiempo_retorno = ticActual + 1;//de todo lo que llevo hasta ahora mas uno sera el tiem
                    terminadoPendiente = centinelaCPU;//guardo el proceso terminado para imprimirlo después
                    centinelaCPU = NULL;//lo vuelvo a poner en null para que alguien mas sea elegido, libero el CPU
                    colaOrigen = -1;//todavia no pertenece a ninguna cola
                }
            }
        }
        ticActual++;//siguiente ronda
        return NULL;//aqui retorno para darle el control a el hilo de Corina
}
void calcular_metricas(PCB procesos[], int total_procesos) {
    if (total_procesos <= 0) return;//si todavia no hay procesos no hago nada
//declaro las variables a usar
    double suma_espera = 0.0;
    double suma_retorno = 0.0;

    printf("\n=== METRICAS DE RENDIMIENTO DEL SISTEMA ===\n");//diseno como lo hacen en los labs
    printf("ID\tLlegada\tBurst\tFin\tTurnaround\tEsperas\n");//diseno solicitado

    for (int i = 0; i < total_procesos; i++) {//hasta terminar con todos los procesos, los centinelas
        int turnaround = procesos[i].tiempo_retorno - procesos[i].tiempo_llegada;//su tiempo de retorno de cada uno
        int espera = turnaround - procesos[i].tiempo_rafaga;//su tiempo de espera es el tiempo de retorno total menor
        if (espera < 0) espera = 0;//si la espera es un numero negativo, solo digo que no espero

        suma_retorno += turnaround;//el total es la suma de cada uno
        suma_espera += espera;//el total es la suma de cada uno
//lo muestro
        printf("%d\t%d\t%d\t%d\t%d\t\t%d\n",
            procesos[i].pid,
            procesos[i].tiempo_llegada,
	        procesos[i].tiempo_rafaga,
            procesos[i].tiempo_retorno,
            turnaround,
            espera);
    }
//variables para sacar los promedios segun las formulas de los libros
    double promedio_retorno = suma_retorno / total_procesos;
    double promedio_espera = suma_espera / total_procesos;
    double throughput = (double)total_procesos / (ticActual > 0 ? ticActual : 1);
//muestro lo que piden
    printf("-------------------------------------------\n");
    printf("Tiempo Medio de Retorno (Turnaround): %.2f tics\n", promedio_retorno);
    printf("Tiempo Medio de Espera (Waiting Time): %.2f tics\n", promedio_espera);
    printf("Rendimiento del Sistema (Throughput):  %.4f proc/tic\n", throughput);
    printf("===========================================\n\n");
}
