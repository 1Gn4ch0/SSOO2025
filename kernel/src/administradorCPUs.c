#include "administradorCPUs.h"

void* administradorCPU(void* arg)
{
	paqueteCPU* paquete = (paqueteCPU*)arg;
	t_log* logger = paquete->logger;
    const char* puertoOrdenes = paquete->puertoOrdenes;
    const char* puertoInterrupcciones = paquete->puertoInterrupciones;

	log_debug(logger, puertoOrdenes);
	log_debug(logger, puertoInterrupcciones);

    int socketOrdenesAbierto = abrirSocket(puertoOrdenes);
    int socketInterrupccionesAbierto = abrirSocket(puertoInterrupcciones);

    int socketOrdenes = conectarCPU(socketOrdenesAbierto);
    int socketInterrupcciones = conectarCPU(socketInterrupccionesAbierto);
    int idCPU = identificarCPU(socketOrdenes);

	bool on = true;
	while(on)
	{
		sem_wait(&semCPUI[idCPU]);
		paqueteDatos* datos = malloc(sizeof(paqueteDatos));
		datos->PID = procesosActivos[idCPU];
		datos->PC = procesosActivos[idCPU];
		//enviarProcesoACPU(socketOrdenes,datos);
		//esperarConfirmacion(socketOrdenes);
		//actualizarPCB(procesosActivos[idCPU]);
		bool ejecucion = true;
		while(ejecucion)
		{
			//SYSCALL[idCPU] = esperarSYSCALL()
			sem_post(&semCPUF[idCPU]);
			sem_wait(&semCPUI[idCPU]);
			//ejecucion = recibirResultadoMain()
			//enviarRespuestaCPU(socketOrdenes);
		}
	}
    

	return EXIT_SUCCESS;
}

int abrirSocket(char *puerto)
{
	int error;
	int socketServidor;
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	error = getaddrinfo(NULL, puerto, &hints, &servinfo);

	socketServidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);//Creamos el socket de escucha del servidor

	//Asociamos el socket a un puerto
	error = setsockopt(socketServidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));//Permitir que un mismo cliente use el mismo port para varias conexiones
	error = bind(socketServidor, servinfo->ai_addr, servinfo->ai_addrlen);//Bindera el socket a un port

	error = listen(socketServidor, SOMAXCONN);//Escuchamos las conexiones entrantes

	freeaddrinfo(servinfo);

	return socketServidor;
}

int conectarCPU(int socketCPUabierto)
{
	int socketClient = accept(socketCPUabierto, NULL, NULL);

	return socketClient;
}

int identificarCPU(int socketCPU)
{
	int32_t id;
	recv(socketCPU, &id, sizeof(int32_t), MSG_WAITALL);
	return id;
}	
