#ifndef ADMINISTRADORIO_H_
#define ADMINISTRADORIO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <semaphore.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>

#include <auxFunctions.h>
#include "mainK.h"


//----------------STRUCT----------------//
typedef struct
{
    char* puertoIO;
    int pos;
    t_log* logger;
} paqueteIO;

typedef struct
{
    int size;
    char* nombre;
} handshake;

//---------------FUNCIONES--------------//
void* administradorIO(void* arg);
int abrirConexion(t_log* logger, char *puerto);
int esperarNuevoDispositivo(t_log* logger, int socketAbierto);
char* identificarDispositivo(t_log* logger, int socketIO);
void enviarOrden(t_log* logger, int socketIO, int pos);
bool recibirConfirmacion(t_log* logger, int socketIO, char* dispositivo);
void desconectarDispositivo(t_log* logger, int socketIO, char* dispositivo, int pos);

#endif