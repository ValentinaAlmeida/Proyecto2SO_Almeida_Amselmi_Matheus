#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <pthread.h>
#include "tcb.h"
//debido a que no se cuanto va crecer la cola 
//Nodo de cada sentinela en la cola 
typedef struct  Node {
  PCB* centinela;//el valor que tendra el centinela 
  struct Node* siguiente;//el que va luego de el sentinela
}Node;//nombre de el nodo 

typedef struct {//inicio de la estructura
    Node* frente;//apunta al primer sentinela que llego
    Node* final;//apunta a el ultimo sentinela que llego
    int tamanio;//tamanio actual de la cola 
    pthread_mutex_t mutex;//semaforo 
}Queue;// se le coloca nombre a la respectia cola 

//Frima de las funciones 

void inicializarQueue(Queue* q);//Funcion que inicializa la Cola, por buenas practicas 
bool isEmpty(Queue* q);//Funcion que verifica si la cola esta vacia
void enqueue(Queue* q, PCB* centinela);//Funcion para insertar un nuevo sentinela al final de la cola 
PCB* dequeue(Queue* q);//Funcion que extrae un sentinela de el frente de la cola
void colocarfrente(Queue* q, PCB* centinela);//Funcion para colocar a alguien en el frente 
int size(Queue* q);//Funcion que te dice cuantos sentinelas hay 
void destruirQueue(Queue* q);//funcion destruye, libera y limpia la cola 
#endif