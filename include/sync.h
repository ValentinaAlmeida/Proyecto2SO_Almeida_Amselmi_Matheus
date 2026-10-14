#ifndef SYNC_H
#define SYNC_H

#include "PCB.h"

int sync_run(int modo_verbose);

void *hilo_maestro(void *arg); //reloj

void *hilo_trabajador(void *arg); //CPU_0

#endif