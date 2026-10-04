#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "PCB.h"
#include "parser.h"

//la parte del parser que se encarga de el txt

int parser_entrada(const char *nombre){//recibo de la entrada el nombre del archivo para el escenario
char *linea=NULL;//donde guardo la linea que leí
size_t tamano = 0;//guardo el tamaño de la línea, y ya getline lo rellena y ajusta segun lo que lea
ssize_t actual;//guardo la cantidad de caracteres que leyó getline(), sie el número es mayor a 0 es que leyó algo
    
    FILE *entrada = fopen(nombre, "r");//abro el archivo de entrada solo para modo lectura
    if(entrada==NULL){
        perror("El archivo no se pudo abrir, vuelva a intentarlo o verifique que el archivo exista");
        return 1;
    }else{
        while((actual = getline(&linea, &tamano, entrada)) != -1){//si leyo algo entonces lo proceso, si regresa -1 es que no leyó nada y puede parar
            if (linea[0] == '\0' || linea[0] == '\n' || linea[0] == '\r'){//espacios o saltos de linea los ignoro
                continue;
            }else if(linea[0] == '#'){ //por si la linea es un comentario
                continue;
            }else{//si verdaderamente la linea esta llena
                PCB temporal;//uso una estructura temporal del tipo PCB para almacenar las cosas y luego copiarlo al arreglo fácilmente
                //variables para guardar las 4 entradas separadas por espacios que dice la entrada
                int t_llegada;
                int pid;
                int prioridad;
                int t_rafaga;
                // leo los valores de la linea y los guardo uno por uno donde corresponde para luego asignarlos bien
                int extraidos = sscanf(linea, "%d %d %d %d", &t_llegada, &pid, &prioridad, &t_rafaga);
                
                if(extraidos==4 && (cantProcesos)<CANTIDAD){//si extrajo completos los 4 valores y esta dentro de mi limite, entonces continuo y guardo todo
                    //guardo lo que leí de la linea donde corresponde y el resto en valores por defecto
                    temporal.pid=pid;
                    temporal.nivel_prioridad=prioridad;
                    temporal.estado= NEW;
                    temporal.tiempo_llegada=t_llegada;
                    temporal.tiempo_restante=t_rafaga;
                    temporal.quantum_consumido=0;
                    temporal.tiempo_espera=0;
                    temporal.tiempo_retorno=0;
                    temporal.tiempo_rafaga=t_rafaga;

                    procesos[cantProcesos]=temporal;
                    cantProcesos+=1;
                }else{
                    perror("El formato de una de las líneas es defectuoso, porfavor ingrese un archivo con todas las líneas correctas");
                    return 1;
                }
            }
        }
    }
    free(linea);//libero la memoria que utilice
    fclose(entrada);//cierro el archivo que lei
    return 0;
}
