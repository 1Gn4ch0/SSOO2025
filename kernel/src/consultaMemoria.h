#ifndef CONSULTAMEMORIA_H_
#define CONSULTAMEMORIA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>


//----------------STRUCT----------------//
typedef struct
{
    int orden;
    int PID;
    int tamañoArchivo;
} PaqueteProceso;

//----------------CLIENT----------------//
int consultaMemoria(t_log* logger, t_config* IOconfig, PaqueteProceso* proceso);
int establecerConexion(char *ip, char* puerto);
void enviarConsulta(t_log* logger, int conexion, PaqueteProceso* proceso);
int esperarConfirmacion(int conexion);

#endif