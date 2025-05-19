#ifndef AUXFUNCTIONS_H_
#define AUXFUNCTIONS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <commons/log.h>
#include <commons/string.h>

char* construirLog(char *a, char *b);
char* ajustarPuerto(char* puerto, int ajuste);

#endif