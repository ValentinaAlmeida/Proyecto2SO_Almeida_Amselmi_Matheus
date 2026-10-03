#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
//Cola de prioridad 0,1,2
Queue colaPrioridad0;//prioridad alta 
Queue colaPrioridad1;//prioridad media 
Queue colaPrioridad2;//prioridad baja 

int ticActual= 0;//contador de tics que leva la simulacion
bool simulacionViva= true;//e este momento el hilo de el CPU trabaja
//Funcion par inicializar las colas a usar
void inicializarScheduler() {//inicalizamos las colas 
    inicializarQueue(&colaPrioridad0);//incializamos la cola de prioridad ala 
    inicializarQueue(&colaPrioridad1);//inicializamos la cola de prioridad media 
    inicializarQueue(&colaPrioridad2);//inicializamos la cola de prioridad baja 
}//fin de la funcion
//Funcion que aplica en round robind y planifica que sentinela toma su puesto 
void* hilo_cpu(void* arg) {//hilo de el cpu que movera los hilos y vera las colas segun la prioridad 
PCB* centinelaCPU = NULL;//quien esta en el CPU
int colaOrigen = -1;//de que cola viene mi centinela 
if (centinelaCPU != NULL && colaOrigen > 0 && !isEmpty(&colaPrioridad0)) {//si actaulemnte tenemos un centinela y la cola de prioridad 0 no esta vacia 
            centinelaCPU->estado = LISTO;//el centinela sera el elegido 
            if (colaOrigen == 1) {//si es la cola de alta prioridad 
                colocarfrente(&colaPrioridad1, centinelaCPU);//lo coloco al frente para que este sea el que se ejecute entonces 
            } else if (colaOrigen == 2) {//si llego uno nuevo de prioridad media se coloca al frnte este recien llegado 
                colocarfrente(&colaPrioridad2, centinelaCPU);//lo colocamos en la cola
            
            centinelaCPU = NULL;//si no e s deninguna cola tenemos un problema , este centinela es un eeror no debria estr aqui 
            colaOrigen = -1;//le colocamos un indicador de que no pertenece aqui 
        }

        if (centinelaCPU == NULL) {//si este centinela no pertece ni prioriada media ni baja 
            if (!isEmpty(&colaPrioridad0)) {//si la cola de prioridades ltas no est vacia 
                centinelaCPU = dequeue(&colaPrioridad0)://este nuevo centinela sers aquel que ya esta esperando con la mas alata prioridad 
                colaOrigen = 0;//indico de donde viene 
            } else if (!isEmpty(&colaPrioridad1)) {//una vez que la col de prioridades altas esta vacia, si esta de prioridades media no esta vacia 
                centinelaCPU = dequeue(&colaPrioridad1);//el elegido sers aquel que ha estado esperando en la cola 
                colaOrigen = 1;//indico de donde viene
            } else if (!isEmpty(&colaPrioridad2)) {//una vez vacia las anteriores y esta de prioridades bajas no lo esta 
                centinelaCPU = dequeue(&colaPrioridad2);//sacouno de la cola
                colaOrigen = 2;//indico de donde viene 
            }
        }
        if (centinelaCPU != NULL) {//si el que esta elegido existe 
            centinelaCPU->estado = EJECUCION;//actualizo su estado en el PCB 

            centinelaCPU->tiempo_restante--;//le quito su tiempo restante porque ya se eta ejecutando disminuye su refga 
            centinelaCPU->quantum_consumido++;//y ya consumio un quantum de su tiempo 

            if (centinelaCPU->tiempo_restante == 0) {//si ya termino 
                centinelaCPU->estado = TERMINADO;//ctaulizo u estado 
                centinelaCPU->tiempo_retorno = ticActual + 1;//de todo lo quellevo hasta ahora mas uno sera el tiempo e su lcio de vida completo 
                
                centinelaCPU = NULL;//lo vuevlo a poner n null para ue alquien mas sea elegido 
                colaOrigen = -1;//todavia no pertenece a ningun cola 
            }//ahora plicamos los round robin, (llevo demasiados if ayuda )
            else if (colaOrigen == 0 && centinelaCPU->quantum_consumido == 2) {//Si es es de alta prioridad su quamtum de tiempo para que pase otro ya se cumpli 
                centinelaCPU->quantum_consumido = 0;//receteo l quantum porque ya lo coumplio y n l proxima iteracio vuelva otea vez hadta nueva,ente llgar a 2 y finalmente termine
                centinelaCPU->estado = LISTO;//listo, vuelve a la cola a esperar su turno
                enqueue(&colaPrioridad0, centinelaCPU);//esta en l col al final esperando su turno 
                
                centinelaCPU = NULL;//le doy chance a otro
                colaOrigen = -1;//todavia no pertenece a ninguna cola 
            } 
            else if (colaOrigen == 1 && centinelaCPU->quantum_consumido == 4) {//Si es es de alta prioridad su quamtum de tiempo para que pase otro ya se cumpli
                centinelaCPU->quantum_consumido = 0;//receteo l quantum porque ya lo coumplio y n l proxima iteracio vuelva otea vez hadta nueva,ente llgar a 4 y finalmente termine
                centinelaCPU->estado = LISTO;//listo, vuelve a la cola a esperar su turno
                enqueue(&colaPrioridad1, centinelaCPU);//esta en l col al final esperando su turno
                
                centinelaCPU = NULL;//le doy chance a otro
                colaOrigen = -1;//todavia no pertenece a ninguna cola
            }
        }

        usleep(100000);//se hace la simulacion de que espera leugo veo sui lo borro o lo dejo 
        ticActual++;//siguiente ronda de ele cpu si hicera un bucle pero eso lo pensare con corina 
        return NULL;
    }

void calcular_metricas(PCB procesos[], int total_procesos) {
    if (total_procesos <= 0) return;//si todavia no hay procesos no hago nada 

    double suma_espera = 0.0;
    double suma_retorno = 0.0;

    printf("\n=== METRICAS DE RENDIMIENTO DEL SISTEMA ===\n");//diseno como lo hacen en los labs 
    printf("ID\tLlegada\tBurst\tFin\tTurnaround\tEsperas\n");//diseno solicitado 

    for (int i = 0; i < total_procesos; i++) {//hasta terminar con todos los proceso, los centinelas 
        int turnaround = procesos[i].tiempo_retorno - procesos[i].tiempo_llegada;//su tiempo de reorn de cada uno es el tiempo de retorno menos el tiempo  lleegada 
        int espera = turnaround - procesos[i].tiempo_rafaga;//su tiempo de espera es el tiempo de reorno toal mno el ytiempo de rafaga que dura cada uno 

        if (espera < 0) espera = 0;//si la espera s un nuemro negativo, solo digo que no espero 

        suma_retorno += turnaround;//el total es la suma de cada uno 
        suma_espera += espera;//el total es la suma de cada uno 
//lo muestro 
        printf("%d\t%d\t%d\t%d\t%d\t\t%d\n",
               procesos[i].id,
               procesos[i].tiempo_llegada,
               procesos[i].tiempo_rafaga,
               procesos[i].tiempo_retorno,
               turnaround,
               espera);
    }
//variables para sacar lso rpomedio segun las forlas de los libros 
    double promedio_retorno = suma_retorno / total_procesos;
    double promedio_espera = suma_espera / total_procesos;
    double throughput = (double)total_procesos / (tic_actual > 0 ? tic_actual : 1);
//muestro lo que piden 
    printf("-------------------------------------------\n");
    printf("Tiempo Medio de Retorno (Turnaround): %.2f tics\n", promedio_retorno);
    printf("Tiempo Medio de Espera (Waiting Time): %.2f tics\n", promedio_espera);
    printf("Rendimiento del Sistema (Throughput):  %.4f proc/tic\n", throughput);
    printf("===========================================\n\n");
}
