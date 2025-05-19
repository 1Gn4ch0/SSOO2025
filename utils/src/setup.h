#ifndef SETUP_H_
#define SETUP_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <commons/log.h>
#include <commons/string.h>


//----------------STRUCT----------------//
typedef struct
{
	char modS;
	char modC;
} modStruct;

//----------------GLOBAL----------------//
char *selectorModulo(char modulo);
char *selectorPuerto(char moduloA, char moduloB);
char *construirLog(char *a, char *b, char *c);

//----------------SERVER----------------//
int conexionServidor(char modS, char modC);
int iniciarServidor(t_log *logger, char *modS, char *modC, char *puerto);
int esperarClient(t_log *logger, int socketServidor, char *modC);
int recibirEntrada(int socketClient);

//----------------CLIENT----------------//
void* conexionCliente(char cli, char ser);
int crearConexion(char *ip, char* puerto);
void enviarHandshake(int conexion);
void terminarPrograma(int conexion, t_log* logger);

#endif
