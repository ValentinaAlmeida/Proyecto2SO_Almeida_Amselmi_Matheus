#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


//Funcion que inicializa la Cola, por buenas practicas 
void inicializarQueue(Queue* q){//recieb un apuntador a la estructura 
 q->frente = NULL;//no hay nada todavia, no hay nadie al frente 
 q->final = NULL;//no hay nadie al final 
 q->tamanio = 0;// todavia no hay  nadie no tiene tamanio
 pthread_mutex_init(&q->mutex, NULL);//inicalizamos el semaforo para cada cola 
}

//Funcion que verifica si la cola esta vacia 
//True= si esta vacia
//False = No esta vacia, tienes sentinelas amigo.
bool isEmpty(Queue * q){
    pthread_mutex_lock(&q->mutex);//tomo mi turno
    bool vacia = (q->frente == NULL);//si no hay nadie , estoy vaicio 
    pthread_mutex_unlock(&q->mutex);//libere, ya hice mi trabajo
    return vacia;
}//final de la funcion 

//Funcion para insertar un nuevo sentinela al final de la cola 
void enqueue( Queue* q, PCB* centinela){//la cola y recibe ael nuevo sentinela 
    pthread_mutex_lock(&q->mutex);//tomo ceerrojo y tomo mi turno
    Node* nodo= (Node*)malloc(sizeof(Node));//reservamos un espacio en el espacio de el heap para un nodo
    if( nodo == NULL){// si fue null es que simplemente no se pudo crear, no le asigno y no tengo direccion a la cual accder
        printf("Error: No se pudo asignar memoria\n");//s emuestra el mensaje 
        pthread_mutex_unlock(&q->mutex);//libero ya termine
        return;//se sale de la funcion 
    }//final de el if
    //una vez qe se verifica que si llegu a este punto fue que porque de verad se reservo lo que se pidio 
    nodo->centinela= centinela;//se agrega el valo de el sentinela 
    nodo->siguiente= NULL;//bien sea que lo asigne al final o si es el primero n ser asignado su siguiente es null
    if(isEmpty(q)){//si esta vacia
        q->frente= nodo;//soy el primero 
        q->final = nodo;//obvio soy el ultimo 
    }else{ //si ya hay sentinelas 
        q->final->siguiente= nodo;//de el ultimo en la cola yo soy el siguiente 
        q->final= nodo;//y ahora el ultimo ya no es el ultimo, soy yo 
    }//final de el else 
    q->tamanio++;//hay alguien mas, contamos el nuevo sentinela que entro 
    pthread_mutex_unlock(&q->mutex);//libero ya termine
}//final de la funcion    

//Funcion que extrae un sentinela de el frente de la cola
PCB* dequeue(Queue* q){//recibo la cola de quien extraigo el sentinela
    pthread_mutex_lock(&q->mutex);//tomo ceerrojo y tomo mi turno
    if (isEmpty(q)) {//s ie sta vacia 
        printf("la cola esta vacia\n");//mostramos mensaje de eeror
        pthread_mutex_unlock(&q->mutex);//libero ya termine
        return NULL;//sal de la funcion, na hay nada que hacer 
    }//fin de el if
    //quien este al frente lo pongo en 
    Node* temp=q->frente;//un nodo temporal 
    PCB* setinelaIdo=temp->centinela;//asigno el sentinela que se va 
    q->frente=q->frente->siguiente;//ahora el frente es el siguiente sentinela en la cola 
    if(q->frente == NULL){//si ya no hay nadie en el frente 
        q->final= NULL;//tampoco hay nadie de ultimo 
    }//final de el if
    free(temp);//libero la memoria que reserve para el sentinela, el ya no esta en la cola 
    q->tamanio--;//un sentinela menos, el tamanio disminuye 
    pthread_mutex_unlock(&q->mutex);//libero ya termine
    return setinelaIdo;//se va el sentienla 
}//fin de la funcion 

//Funcon para saber la cantidad de setinelas actuales 
int size(Queue* q) {//recieb la cola 
   pthread_mutex_lock(&q->mutex);//tomo ceerrojo y tomo mi turno
    int s = q->tamanio;//miro cuantos hay 
    pthread_mutex_unlock(&q->mutex);//libero ya termine 
    return s;//retorno cuantos hay 
}//finde la funcion 

//Funcion para colocar a alguien en el frente 

void colocarfrente(Queue* q, PCB* centinela){//rcobo al sentinela y a la cola 
    pthread_mutex_lock(&q->mutex);//tomo ceerrojo y tomo mi turno
    Node* nodo= (Node*)malloc(sizeof(Node));//reservamos un espacio en el espacio de el heap para un nodo
    if( nodo == NULL){// si fue null es que simplemente no se pudo crear, no le asigno y no tengo direccion a la cual accder
        printf("Error: No se pudo asignar memoria\n");//s emuestra el mensaje 
       pthread_mutex_unlock(&q->mutex);//libero ya termine
        return;//se sale de la funcion 
    }//final de el if
    nodo->centinela = centinela;//inicalizo a el sentinela en su lugar 
    nodo->siguiente = q->frente; // El nuevo nodo apunta al antiguo frente
    q->frente = nodo;//ahora el feene es el nuevo que vino
    // Si la cola estaba vacia, tambien pasa a ser el final
    if (q->final == NULL) {
        q->final = nodo;
    }
q->tamanio++;//hay alguien mas, contamos el nuevo sentinela que entro 
pthread_mutex_unlock(&q->mutex);//libero ya termine
}//fin de la funcion 
//funcion para eliminar la cola una vez que ya no sea nesesaria 
void destruirQueue(Queue * q){
    pthread_mutex_lock(&q->mutex);//tomo ceerrojo y tomo mi turno
    Node* temp;//un nodo temporal paa ir guarando a el siguiente a eliminar 
    while(q->frente != NULL){//si esta vacia 
        temp=q->frente;//prmero el inicio 
        q->frente=temp->siguiente;//el incio sera el siguiente sentinela a morir 
        free(temp);//lo liberamos
    }//ya no hya nadie aqui 
    q->final=NULL;//ya no hay nadie, no hay ultimo
    q->tamanio=0;//no hay naide, no hay tmanio 
    pthread_mutex_unlock(&q->mutex);//libero ya termine
    pthread_mutex_destroy(&q->mutex);//destruimos el cerrojo que usamos
}