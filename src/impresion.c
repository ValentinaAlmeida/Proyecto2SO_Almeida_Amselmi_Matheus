#include <stdio.h>
#include "impresion.h"

static int verbose_activo = 0;//ver si esta en verbose
static const char *TIPOS[3] = {"Command", "Driller", "Heavy"};

//extraer el modo verbose para imprimir
void set_modo_verbose(int valor) {
    verbose_activo = valor;
}

int get_modo_verbose(void) {
    return verbose_activo;
}

//para admitir llegadas y el scheduler
void impresionIndividual(int tic, Evento evento, const PCB *p, int parametroExtra) {
    if (p == NULL){//si es nulo retorno
        return;
    } 
    const char *eventoSucedido = "Unknown";//por defecto lo desconozco y despues lo lleno
    if((p->nivel_prioridad>=0) && (p->nivel_prioridad<=2)){//verifico los niveles de prioridad según se definio
        eventoSucedido=TIPOS[p->nivel_prioridad];//pongo el nombre segun el pdf, y hago la conversión del nivel de prioridad (0, 1, 2)
    }
    //Ahora pongo el formato de impresion que pidieron en el pdf
    switch (evento) {//veo cual es el evento e imprimo segun lo que toque
        case TIEMPO_LLEGADA://si acaba de llegar
            printf("[T-%04d] ARRIVAL: Sentinel ID:%d [Type: %s]\n", tic, p->pid, eventoSucedido);
            break;
        case QUANTUM_EXPIRADO://se le expiro el quantum, se hace interrupción de reloj
            printf("[T-%04d] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=%d)\n", tic, p->pid, parametroExtra);
            break;
        case EXPROPIECE://uno de mayor prioridad expropia al que esta en el CPU
            printf("[T-%04d] PREEMPTION: Sentinel %d preempted by higher priority task\n", tic, p->pid);
            break;
        case FINALIZADO: { //ya finalizó el sentinela/proceso
            int turnaround = p->tiempo_retorno - p->tiempo_llegada;
            printf("[T-%04d] COMPLETED: Sentinel ID:%d -> Turnaround Time: %d ticks\n", tic, p->pid, turnaround);
            break;
        }
    }
}

// Impresiones del verbose, que estamos asumiendo
void impresionVerboseTick(int tic, const PCB *cpu_p, int cola_origen) {
    if (!verbose_activo){//si no esta en verbose no tiene sentido que entre
        return;
    }

    // uso las funciones de las colas para imprimir
    int q0_size = size(&colaPrioridad0);
    int q1_size = size(&colaPrioridad1);
    int q2_size = size(&colaPrioridad2);
    //uso mi formato porque no hay
    printf("[VERB-T%04d] Estado Colas -> Q0(Cmd):%d | Q1(Drill):%d | Q2(Heavy):%d | CPU: ", tic, q0_size, q1_size, q2_size);

    if (cpu_p != NULL) {
        const char *tipo_cpu = "Unknown";//por defecto lo desconozco y despues lo lleno
        if(cpu_p->nivel_prioridad >= 0 && cpu_p->nivel_prioridad <= 3){//verifico los niveles de prioridad según se definio
            tipo_cpu = TIPOS[cpu_p->nivel_prioridad];//pongo el nombre segun el pdf, y hago la conversión del nivel de prioridad (0, 1, 2)
        }

        printf("Ejecutando ID:%d [%s] (Restante:%d, Quantum Consumido:%d)\n", cpu_p->pid, tipo_cpu, cpu_p->tiempo_restante, cpu_p->quantum_consumido);
    } else {
        (void)cola_origen;
        printf("IDLE (Libre)\n");
    }
}

//esta para el MAIN
void impresionCompleta(const PCB procesos[], int total_procesos, int total) {
    if (total_procesos<= 0) {
        return;
    }
    double suma=0.0;//temporal para almacenar la suma de esperas
    for (int i=0;i<total_procesos;i++){//calculo el tiempo desde que llegó hasta que termino, y obtengo las esperas intermedias y así puedo tener finalmente la suma de todas las esperas
        int turnaround=procesos[i].tiempo_retorno - procesos[i].tiempo_llegada;
        int espera=turnaround - procesos[i].tiempo_rafaga;
        if (espera<0){//si es negativo simplemente lo vuelvo 0, caso borde solo por si acaso, no deberia de pasar, en teoria
            espera=0;
        }
        suma=suma+espera;
    }

    double waiting_time=suma/total_procesos;//saco el tiempo de espera promedio por la formula de clase
    if(total <= 0){//Verifico si el total es 0 o negativo, para no afectar la division y no caer en el caso de division entre 0
        total=1;
    }
    double throughput = (double)total_procesos / total;//cantidad de procesos terminados por unidad de tiempo
    //Imprimo finalment todo
    printf("\n==================== GLOBAL PERFORMANCE METRICS ====================\n");
    printf("\t- Total Simulation Time : %d clock ticks.\n", total);
    printf("\t- System Throughput     : %.2f sentinels/tick.\n", throughput);
    printf("\t- Average Waiting Time  : %.2f ticks.\n", waiting_time);
    printf("====================================================================\n");
}