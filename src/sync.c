#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "PCB.h"
#include "queue.h"
#include "scheduler.h"
#include "sync.h"
#include "impresion.h"

static sem_t sem_admision; //indica si ya admitio llegadas
static sem_t sem_tic; //indica si ya se ejecuto el tic

static int fin=0; //el trabajador la pone en 1 cuando ya terminaron todos los centinelas, y el maestro lo lee
static int modo_verbose=0; 
static int *orden = NULL; //arreglo con los índices de procesos ordenados por tiempo de llegada
static int siguiente = 0; // marca cual es el próximo centinela por admitir

//Bloquea hasta que el semaforo sea mayor que 0 y si una señal del sistema lo interrumpe devuelve -1
static void esperar(sem_t *s){
    while(sem_wait(s) == -1 && errno == EINTR){}
}

//Convierte el numero de cola (0, 1, 2) en un puntero a la cola
static Queue *cola_prioridad(int nivel){
    switch(nivel){
        case 0:  return &colaPrioridad0;
        case 1:  return &colaPrioridad1;
        case 2:  return &colaPrioridad2;
        default: return NULL;
    }
}

//Comparador para qsort, ordena indices por tiempo de llegada, devuelve -1, 0 o 1
static int cmp_llegada(const void *a, const void *b){
    // qsort entrega punteros genericos, convierten a int* y lee el indice
    int ia = *(const int *)a, ib = *(const int *)b;
    if(procesos[ia].tiempo_llegada != procesos[ib].tiempo_llegada)
        return procesos[ia].tiempo_llegada < procesos[ib].tiempo_llegada ? -1 : 1;
    return (ia > ib) - (ia < ib);
}

// Admite todos los procesos cuyo tiempo de llegada ya se cumplio
static void admitir_llegadas(int tg){
    while(siguiente < cantProcesos && procesos[orden[siguiente]].tiempo_llegada <= tg){
        PCB *p = &procesos[orden[siguiente]]; // puntero al PCB del proximo proceso en orden de llegada
        siguiente++;
        p->estado = READY;
        enqueue(cola_prioridad(p->nivel_prioridad), p);
        impresionIndividual(tg, TIEMPO_LLEGADA, p, 0);
    }
}

// Hilo maestro (reloj): admite llegadas y marca el ritmo de los tics
void *hilo_maestro(void *arg){
    (void)arg; // evita el warning del compilador
    for(;;){ // bucle infinito, una vuelta por tic
        admitir_llegadas(ticActual);  // encola los procesos que llegan en este tic
        sem_post(&sem_admision);
        esperar(&sem_tic); // se bloquea hasta que el trabajador termine el tic
        if(fin) break; // si ya terminaron todos los procesos, sale del bucle
    }
    return NULL;
}

// Hilo trabajador (CPU): ejecuta un tic del planificador por cada vuelta
void *hilo_trabajador(void *arg){
    (void)arg; // evita el warning del compilador
    int terminados = 0; // contador de procesos en estado FINISHED
    for(;;){ // bucle infinito, una vuelta por tic
        esperar(&sem_admision); // espera a que el maestro haya admitido las llegadas
        hilo_cpu(NULL); // ejecuta un tic de CPU
        terminados = 0;
        for(int i = 0; i < cantProcesos; i++) // recorre todos los procesos
            if(procesos[i].estado == FINISHED) terminados++;   // cuenta los que ya terminaron
        if(terminados >= cantProcesos) fin = 1; // si terminaron todos los procesos, activa la bandera de fin
        if(fin) imprimirFinalizacionPendiente();
        sem_post(&sem_tic);
        if(fin) break; // si ya terminaron todos los procesos, sale del bucle
    }
    return NULL;
}


int sync_run(int verbose){
    modo_verbose = verbose;
    set_modo_verbose(verbose);
    if(cantProcesos <= 0){ // valida que haya al menos un centinela
        fprintf(stderr, "Error: el archivo de entrada no contiene centinelas\n");
        return 1;
    }

    // reserva memoria para cantProcesos
    orden = malloc((size_t)cantProcesos * sizeof(int));
    if(orden == NULL){ // si malloc falla
        perror("No se pudo asignar memoria para el orden de llegadas");
        return 1;
    }
    for(int i=0; i<cantProcesos; i++) orden[i] = i; // llena con 0,1,2,...,n-1
    qsort(orden, (size_t)cantProcesos, sizeof(int), cmp_llegada);

    ticActual = 0; 
    siguiente = 0;
    fin = 0;
    sem_init(&sem_admision, 0, 0);
    sem_init(&sem_tic, 0, 0);

    pthread_t h_cpu, h_reloj;
    int estado = 0; // codigo de retorno, 0=exito
    if(pthread_create(&h_cpu, NULL, hilo_trabajador, NULL) != 0){ // crea el hilo trabajador
        fprintf(stderr, "Error: no se pudo crear el hilo trabajador\n");
        estado = 1;
    }else if(pthread_create(&h_reloj, NULL, hilo_maestro, NULL) != 0){  // crea el hilo maestro
        fprintf(stderr, "Error: no se pudo crear el hilo maestro\n");
        pthread_cancel(h_cpu);
        pthread_join(h_cpu, NULL);
        estado = 1;
    }else{
        pthread_join(h_reloj, NULL); // espera a que el maestro termine
        pthread_join(h_cpu, NULL); // espera a que el trabajador termine
    }

    sem_destroy(&sem_admision);
    sem_destroy(&sem_tic);
    free(orden);
    orden = NULL;
    return estado;
}