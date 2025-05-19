#include "mainK.h"

// los free() quizas causen problemas

pthread_t CPUs[4];
int IDCPUs[4];

pthread_t IOs[10];
pthread_t mainIO;
hilosDispositivos dispositivos[10];
t_queue* bloqPorIO[10];
sem_t semDispoI[10];
sem_t semDispoF[10];
paqueteOrden* procesoIO[10];
sem_t contadorDispositivos;
sem_t mutexIO[10];

int planificadorLargo; //0=FIFO / 1=SJF
bool planificadorCorto;	//0=FIFO / 1=SJF(sin) / 2=SJF(con)
int procesosIniciados;

t_queue* colaNEW;
t_queue* colaREADY;
t_queue* colaSUSPREADY;

t_config* Kconfig;
t_log* logger;

int main(void) 
{
	//-----------------Set Up-----------------//
	Kconfig = config_create("kernel.config");
	config_get_string_value(Kconfig, "LOG_LEVEL");
	logger = log_create("KERNEL.log", "KERNEL", 1, LOG_LEVEL_DEBUG);

	colaNEW = queue_create();
	colaREADY = queue_create();
	colaSUSPREADY = queue_create();
	procesosIniciados = 0;

	//CPU
	char* puertoCPU1 = config_get_string_value(Kconfig, "PUERTO_ESCUCHA_DISPATCH");
	char* puertoCPU2 = config_get_string_value(Kconfig, "PUERTO_ESCUCHA_INTERRUPT");
	paqueteCPU* variablesHilo = malloc(sizeof(paqueteCPU));
	variablesHilo->logger = logger;
	int f1;
	for(f1=0; f1<4; f1++){
		variablesHilo->puertoOrdenes = ajustarPuerto(puertoCPU1,f1);
		variablesHilo->puertoInterrupciones = ajustarPuerto(puertoCPU2,f1);
		pthread_create(&CPUs[f1], NULL, administradorCPU, variablesHilo);
		pthread_detach(CPUs[f1]);
	}
	free(variablesHilo);

	//IO
	paqueteIO* paqueteMain = malloc(sizeof(paqueteIO));
	paqueteMain->puertoIO = config_get_string_value(Kconfig, "PUERTO_ESCUCHA_IO");
	paqueteMain->logger = logger;
	
	sem_init(&contadorDispositivos, 0, 10);
	pthread_create(&mainIO, NULL, adminIO, paqueteMain);
	pthread_detach(mainIO);
	free(paqueteMain);

	int f2;
	for(f2=0; f2<10; f2++){
		bloqPorIO[f2] = queue_create();
		sem_init(&semDispoI[f2], 0, 0);
		sem_init(&semDispoF[f2], 0, 0);
		sem_init(&mutexIO[f2], 0, 1);
	}
	//---------------------------------------//
	
	log_info(logger, "Iniciando Kernel...");

	char* archivoInicial;
	int tamanioArchivoInicial;

	

	

	
	/*
	Quehaceres:
		-checkear que le envia el cpu al kernel para iniciar un proceso
		-realizar las syscalls
		-logs obligatorios
		-semaforos para sincronizar (y bloquear?) procesos
	*/



	

	//-----------------Liberacion de memoria-----------------//
	log_info(logger, "Finalizando Kernel...");
	log_destroy(logger);
	//sem_destroy(& );

	return EXIT_SUCCESS;
}


//-----------------------------FUNCIONES-----------------------------//
void* adminIO(void* arg)
{
	paqueteIO* paqueteMain = (paqueteIO*)arg;
	while(true)
	{
		sem_wait(&contadorDispositivos);
		int pos;
		for(int f=0; f<10; f++)
		{
			if(dispositivos[f].enUso != true)
			{
				pos = f;
				f = 10;
			}
		}
		dispositivos[pos].enUso = true;

		paqueteIO* paqueteDisp = malloc(sizeof(paqueteIO));
		paqueteDisp->logger = paqueteMain->logger;
		paqueteDisp->pos = pos;
		paqueteDisp->puertoIO = ajustarPuerto(paqueteMain->puertoIO,pos);

		pthread_create(&IOs[pos], NULL, administradorIO, paqueteDisp);
		pthread_detach(IOs[pos]);
		free(paqueteDisp);
	}
}

PCB* iniciarProceso(char* nombreArchivo, int tamanioArchivo)
{
	PCB* procesoNuevo = malloc(sizeof(PCB));
	procesoNuevo->PID = procesosIniciados;
	procesoNuevo->PC = 0;
	for(int f=0; f<7; f++){
		procesoNuevo->ME[f] = 0;
		procesoNuevo->MT[f] = 0;
	}
	procesoNuevo->state = 0;
	procesoNuevo->size = tamanioArchivo;
	procesoNuevo->name = nombreArchivo;
	procesosIniciados++;
	return procesoNuevo;
}

void planificadorLargoPlazo(t_queue* cola, PCB* nuevoElemento)
{
	switch (planificadorLargo){
	case 0:
		FIFO(cola, nuevoElemento);
		break;
	default:
		//SJF
		break;
	}
}

void planificadorCortoPlazo(t_queue* cola, PCB* nuevoElemento)
{
	switch (planificadorCorto){
	case 0:
		FIFO(cola, nuevoElemento);
		break;
	case 1:
		//SJF(sin)
		break;
	default:
		//SJF(con)
		break;
	}
}

void FIFO(t_queue* cola, PCB* nuevoElemento)
{
	queue_push(cola, nuevoElemento);
}

bool consultarEntradaProceso(PCB* proceso){
	PaqueteProceso* paqueteMemoria = malloc(sizeof(PaqueteProceso));
	paqueteMemoria->orden = 0;
	paqueteMemoria->tamañoArchivo = proceso->size;
	paqueteMemoria->PID = proceso->PID;
	int confirmacion = consultaMemoria(logger, Kconfig, paqueteMemoria);
	//free(&PaqueteProceso);
	if(confirmacion==0){
		return true;
	}else{
		return false;
	}
}


//------------------------------SYSCALLS------------------------------//
void INIT_PROC(char* nombreArchivo, int tamanioArchivo)
{
	PCB* procesoNuevo = iniciarProceso(nombreArchivo,tamanioArchivo);
	if(queue_is_empty(colaNEW)){
		bool confirmacion = consultarEntradaProceso(procesoNuevo);
		if(confirmacion){
			procesoNuevo->state = 1;
			procesoNuevo->ME[1]++;
			planificadorCortoPlazo(colaREADY, procesoNuevo);
		}else{
			procesoNuevo->state = 0;
			procesoNuevo->ME[0]++;
			planificadorLargoPlazo(colaNEW, procesoNuevo);
		}
	}else{
		procesoNuevo->state = 0;
		procesoNuevo->ME[0]++;
		planificadorLargoPlazo(colaNEW, procesoNuevo);
	}
}

void EXIT(PCB* proceso)
{
	proceso->state = 6;
	proceso->ME[6]++;

	int confirmacion = 1;
	PaqueteProceso* paqueteMemoria = malloc(sizeof(PaqueteProceso));
	paqueteMemoria->orden = 1;
	paqueteMemoria->tamañoArchivo = proceso->size;
	paqueteMemoria->PID = proceso->PID;
	while(confirmacion == 1){
		confirmacion = consultaMemoria(logger, Kconfig, paqueteMemoria);
		if(confirmacion == 0){
			free(proceso);
			if(queue_is_empty(colaSUSPREADY))
			{
				bool entraProNuevo = consultarEntradaProceso(queue_peek(colaNEW));
				if(entraProNuevo){
					PCB* procesoTemp = queue_peek(colaNEW);
					queue_pop(colaNEW);
					procesoTemp->state = 1;
					procesoTemp->ME[1]++;
					planificadorCortoPlazo(colaREADY,procesoTemp);
				}
			}
			else
			{
				bool entraProSusp = consultarEntradaProceso(queue_peek(colaSUSPREADY));
				if(entraProSusp){
					PCB* procesoTemp = queue_peek(colaSUSPREADY);
					queue_pop(colaSUSPREADY);
					procesoTemp->state = 1;
					procesoTemp->ME[1]++;
					planificadorCortoPlazo(colaREADY,procesoTemp);  //es mediano, esto es un placeholder
				}
			}
		}
		else{
			log_error(logger, "Error al confirmar finalizacion de proceso, reintentando");
		}
	}
	free(paqueteMemoria);
}

void DUMP_MEMORY(PCB* proceso)
{
	proceso->state = 3;
	proceso->ME[3]++;

	PaqueteProceso* paqueteMemoria = malloc(sizeof(PaqueteProceso));
	paqueteMemoria->orden = 2;
	paqueteMemoria->tamañoArchivo = proceso->size;
	paqueteMemoria->PID = proceso->PID;
	int confirmacion = consultaMemoria(logger, Kconfig, paqueteMemoria);
	if(confirmacion==0){
		proceso->state = 1;
		proceso->ME[1]++;
		planificadorCortoPlazo(colaREADY, proceso); //es mediano, esto es un placeholder
	}else{
		EXIT(proceso);
	}
}

void IO(PCB* proceso, char* dispositivo, int duracion, int orden)
{
	int pos = 10;
	for(int f=0; f<10; f++){
		if(dispositivo == dispositivos[f].nombre){
			pos = f;
			f = 10;
		}
	}
	if(pos =! 10)
	{
		proceso->state = 3;
		proceso->ME[3]++;
		queue_push(bloqPorIO[pos],proceso);
		sem_wait(&mutexIO[pos]);
		procesoIO[pos]->orden = orden;
		procesoIO[pos]->duracion = duracion;
		procesoIO[pos]->PID = proceso->PID;
		sem_post(&semDispoI[pos]);
		sem_wait(&semDispoF[pos]);
		sem_post(&mutexIO[pos]);
		queue_pop(bloqPorIO[pos]);
		if(dispositivos[pos].enUso == false){
			EXIT(proceso);
		}
	}
	else{EXIT(proceso);}
}








/*
"Proceso pasa a estado EXEC"
ejecutarProceso()



*/
















/*
void leerComando(t_log* logger)
{
	char* leido;
	leido = readline(">");
	switch (*leido) {
	case 'i':
		log_info(logger, "I/O");
		free(leido);
		conexionServidor('k','i');
		break;	
	case 'm':
		log_info(logger, "Memoria");
		free(leido);
		conexionCliente('k','m');
		break;
	case 'c':
		log_info(logger, "CPU");
		free(leido);
		conexionServidor('k','c');
		break;
	default:
		log_info(logger, "Comando desconocido");
		free(leido);

		break;
	}
}
*/