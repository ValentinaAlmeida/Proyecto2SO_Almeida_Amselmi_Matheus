#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "PCB.h"
#include "queue.h"
#include "scheduler.h"
#include "sync.h"

static sem_t sem_admision; //indica si ya admitio llegadas
static sem_t sem_tic; //indica si ya se ejecuto el tic

static int fin=0; //el trabajador la pone en 1 cuando ya terminaron todos los centinelas, y el maestro lo lee
static int modo_verbose=0; //CREO que esta variable no esta haciendo nada, pero la dejo por si acaso
static int *orden = NULL; //arreglo con los índices de procesos ordenados por tiempo de llegada
static int siguiente = 0; // marca cual es el próximo centinela por admitir
static const char *TIPO[3] = {"Command", "Driller", "Heavy"}; //para el mensaje ARRIVAL

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

static int cmp_llegada(const void *a, const void *b){
    int ia = *(const int *)a, ib = *(const int *)b;
    if(procesos[ia].tiempo_llegada != procesos[ib].tiempo_llegada)
        return procesos[ia].tiempo_llegada < procesos[ib].tiempo_llegada ? -1 : 1;
    return (ia > ib) - (ia < ib);
}

static void admitir_llegadas(int tg){
    while(siguiente < cantProcesos && procesos[orden[siguiente]].tiempo_llegada <= tg){
        PCB *p = &procesos[orden[siguiente]];
        siguiente++;
        p->estado = READY;
        enqueue(cola_prioridad(p->nivel_prioridad), p);
        printf("[T-%04d] ARRIVAL: Sentinel ID:%d [Type: %s]\n", tg, p->pid, TIPO[p->nivel_prioridad]);
    }
}

void *hilo_maestro(void *arg){
    (void)arg;
    for(;;){
        admitir_llegadas(ticActual);
        sem_post(&sem_admision);
        esperar(&sem_tic);
        if(fin) break;
    }
    return NULL;
}

void *hilo_trabajador(void *arg){
    (void)arg;
    int terminados = 0;
    for(;;){
        esperar(&sem_admision);
        hilo_cpu(NULL);
        terminados = 0;
        for(int i = 0; i < cantProcesos; i++)
        if(procesos[i].estado == FINISHED) terminados++;    
        if(terminados >= cantProcesos) fin = 1;
        sem_post(&sem_tic);
        if(fin) break;
    }
    return NULL;
}

int sync_run(int verbose){
    modo_verbose = verbose;

    if(cantProcesos <= 0){
        fprintf(stderr, "Error: el archivo de entrada no contiene centinelas\n");
        return 1;
    }

    orden = malloc((size_t)cantProcesos * sizeof(int));
    if(orden == NULL){
        perror("No se pudo asignar memoria para el orden de llegadas");
        return 1;
    }
    for(int i=0; i<cantProcesos; i++) orden[i] = i;
    qsort(orden, (size_t)cantProcesos, sizeof(int), cmp_llegada);

    ticActual = 0;
    siguiente = 0;
    fin = 0;
    sem_init(&sem_admision, 0, 0);
    sem_init(&sem_tic, 0, 0);

    pthread_t h_cpu, h_reloj;
    int estado = 0;
    if(pthread_create(&h_cpu, NULL, hilo_trabajador, NULL) != 0){
        fprintf(stderr, "Error: no se pudo crear el hilo trabajador\n");
        estado = 1;
    }else if(pthread_create(&h_reloj, NULL, hilo_maestro, NULL) != 0){
        fprintf(stderr, "Error: no se pudo crear el hilo maestro\n");
        pthread_cancel(h_cpu);
        pthread_join(h_cpu, NULL);
        estado = 1;
    }else{
        pthread_join(h_reloj, NULL);
        pthread_join(h_cpu, NULL);
    }

    sem_destroy(&sem_admision);
    sem_destroy(&sem_tic);
    free(orden);
    orden = NULL;
    return estado;
}