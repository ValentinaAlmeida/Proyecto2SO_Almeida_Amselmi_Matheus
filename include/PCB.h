#ifndef PCB_H
#define PCB_H

#define CANTIDAD 256

typedef enum {//enumerado que contiene todos los estados
    NEW, 
    READY, 
    RUNNING, 
    INTERRUPTED, 
    FINISHED
} Estado;

typedef struct {//estructura del PCB de cada proceso
	int pid;//ID del proceso para poder ubicarlo adecuadamente
    int nivel_prioridad;//prioridad actual del proceso
    Estado estado; //NEW, READY, RUNNING, INTERRUPTED, FINISHED
	int tiempo_llegada;//cuando llegó
	int tiempo_rafaga;//cuanto tiempo se tiene que ejecutar para estar terminado
    int tiempo_restante;//cuanto le falta para terminar
    int quantum_consumido;//quantum que consumio
	int tiempo_espera;//cuanto tiempo estuvo esperando en la cola de listos
    int tiempo_retorno;//tiempo desde que llegó hasta que culmina el proceso
} PCB;

extern PCB procesos[CANTIDAD];
extern int cantProcesos;

#endif