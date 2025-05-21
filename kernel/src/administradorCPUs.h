#ifndef ADMINISTRADORCPUS_H_
#define ADMINISTRADORCPUS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <semaphore.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <auxFunctions.h>
#include "mainK.h"


//----------------STRUCT----------------//
typedef struct 
{
    char* puertoOrdenes;
    char* puertoInterrupciones;
    t_log* logger;
} paqueteCPU;

typedef struct 
{
    int PID;
    int PC;
} paqueteDatos;

//---------------FUNCIONES--------------//
void* administradorCPU(void* arg);
int abrirSocket(char *puerto);
int conectarCPU(int socketCPUabierto);
int identificarCPU(int socketCPU);


#endif