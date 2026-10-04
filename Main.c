#include <stdio.h>
#include <string.h>
#include "PCB.h"
#include "parser.h"

PCB procesos[CANTIDAD];//defino la estructura que guarda los pcb de todos los procesos
int cantProcesos = 0;//variable global para saber cuantos de los 128 voy a usar, en caso de querer usar for

int main(int argc, char *argv[]){//para recibir los argumentos por terminal del nombre del archivo txt y si tiene activado la bandera de verbose
    char *argumentos=NULL;//nombre del archivo
    int modo_v=0;//bandera para el modo verbose
    
    for(int i=1;i<argc;i++){//recorro los argumentos a partir del 1ro ya que el 0 es el nombre del ejecutable
        if(strcmp(argv[i],"-v")==0){
            modo_v=1;//enciendo la bandera
        }else if(strcmp(argv[i],"--verbose")==0){
            modo_v=1;//enciendo la bandera
        }else if(strcmp(argv[i],"--input")==0){//ahora se supone que luego de el viene el nombre del archivo txt
            if(i+1>=argc){//si al sumarle 1 al indice ya no hay un siguiente argumento, es que la entrada estaba mala y le falta el nombre del txt
                perror("No se ha colocado el nombre del archivo");  
                return 1; 
            }else{
                argumentos=argv[i+1];//guardo el nombre del txt en el arreglo de caracteres para poder mandarselo al parser
                i++;//aumento el contador para que se salga del bucle en la sig iteración
            }
        }
        else{
            perror("Ha ocurrido un error con respecto a la línea ingresada por terminal, por favor verifique y vuelva a intentarlo");
            return 1;
        }
    }

    if(argumentos==NULL){//verifico si se guardo bien el nombre del txt
        perror("El nombre del archivo de entrada no es valido, por favor vuelva a ejecutar");
        return 1;
    }
    int verificar= parser_entrada(argumentos); //envio al parser el nombre del archivo del que sacará los datos

    if(verificar==1){//si ocurrió alguna clase de error en el parser
        perror("Se finaliza la ejecución por la ocurrencia de algún error en la lectura del archivo");
        return 1;
    }else{//si no ocurrió ningún error en el parser, ya fueron guardados todos los PCB
        //se supone que aquí se llama a los algoritmos
        
    }
    
}

