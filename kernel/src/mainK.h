#ifndef MAINK_H_
#define MAINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/queue.h>

#include "administradorIO.h"
#include "administradorCPUs.h"
#include "consultaMemoria.h"


//---------------STRUCTS----------------//
typedef struct
{
	int PID;
	int PC;
    int ME[7];
    int MT[7];
    int state; // NEW=0 / READY=1 / EXEC=2 / BLOCKED=3 / SUSPBLOCKED=4 / SUSPREADY=5 / EXIT=6
    int size;
    char* name;
} PCB;

typedef struct
{
    bool enUso;
    char* nombre;
} hilosDispositivos;

typedef struct
{
    int duracion;
    int orden;
    int PID;
} paqueteOrden;

//---------------FUNCIONES--------------//
void* adminIO(void* arg);
PCB* iniciarProceso(char* nombreArchivo, int tamanioArchivo);
void planificadorLargoPlazo(t_queue* cola, PCB* nuevoElemento);
void planificadorCortoPlazo(t_queue* cola, PCB* nuevoElemento);
void FIFO(t_queue* cola, PCB* nuevoElemento);
bool consultarEntradaProceso(PCB* proceso);

//---------------SYSCALLS---------------//
void INIT_PROC(char* nombreArchivo, int tamanioArchivo);
void EXIT(PCB* proceso);
void DUMP_MEMORY(PCB* proceso);
void IO(PCB* proceso, char* dispositivo, int duracion, int orden);

//----------VARIABLES GLOBALES----------//
extern sem_t mutIn1;
extern sem_t mutIn2;
extern sem_t mutIn3;

extern hilosDispositivos dispositivos[10];
extern t_queue* bloqPorIO[10];
extern sem_t semDispoI[10];
extern sem_t semDispoF[10];
extern paqueteOrden* procesoIO[10];
extern sem_t contadorDispositivos;

#endif