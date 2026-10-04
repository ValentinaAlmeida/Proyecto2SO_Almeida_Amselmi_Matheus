#include <stdio.h>
#include "PCB.h"
#include "parser.h"

PCB procesos[CANTIDAD];
int cantProcesos = 0;

int main(){

    //okay aqui deberia ir el while, pero aun no lo he puesto :)
    int verificar= parser_entrada(procesar_terminal()); 

    if(verificar==1){
        perror("Se finaliza la ejecución por la ocurrencia de algún error en la lectura del archivo");
        return 1;
    }else{
        
    }
    
}

