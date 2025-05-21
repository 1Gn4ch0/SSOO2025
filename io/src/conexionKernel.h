#ifndef CONEXIONKERNEL_H_
#define CONEXIONKERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <auxFunctions.h>

//----------------STRUCT----------------//
typedef struct
{
    int duracion;
    int orden;
    int PID;
} paqueteOrden;

typedef struct
{
    int size;
    void* nombre;
} handshake;

//---------------FUNCIONES--------------//
int conexionKernel(char* dispositivo, t_log* logger, t_config* IOconfig);
int conectarDispositivo(char *ip, char* puerto);
void enviarHandshakeIO(int conexion, char* dispositivo);
paqueteOrden* esperarRespuestaIO(int conexion, t_log* logger);
int ejecutarOrdenIO(int conexion, t_log* logger, paqueteOrden* orden);
void confirmacionDeOrden(int conexion, int32_t confirmacion);
void desconectarDispositivo(int conexion, t_log* logger);

#endif