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

	while(true)
	{
		sem_wait(&semCPUI[idCPU]); //espera el aviso de adminCPU para iniciar
		paqueteDatos* datos = malloc(sizeof(paqueteDatos));
		datos->PID = procesosActivos[idCPU];
		datos->PC = procesosActivos[idCPU];

		bool confirmacion = 0;
		while(confirmacion == 0)
		{
			confirmacion = enviarProcesoACPU(socketOrdenes,datos); //enviar el proceso a CPU y espera su confirmacion
		}
		procesosActivos[idCPU]->state = 2;
		procesosActivos[idCPU]->ME[2]++;
		//log_info(logger, construirLog(string_aitoa(datos->PID), "/Pasa de estado READY al estado EXEC"));

		ejecucion[idCPU] = true; //asegura la ejecucion actual del proceso, en caso de que el proceso termine o sea bloqueado, pasara a false
		while(ejecucion[idCPU])
		{
			SYSCALL[idCPU] = esperarSYSCALL(socketOrdenes);
			sem_post(&semCPUF[idCPU]); //envia señal a adminCPU para ejecutar la SYSCALL
			sem_wait(&semCPUI[idCPU]); //espera que se ejecute la SYSCALL
			enviarRespuestaCPU(socketOrdenes); //envia una confirmacion al CPU
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

int enviarProcesoACPU(int socketOrdenes, paqueteDatos* datos)
{
	int confirmacion;
	send(socketOrdenes, datos, 2*sizeof(int), 0);
	recv(socketOrdenes, &confirmacion, sizeof(int), MSG_WAITALL);
	return confirmacion;
}

struct paqueteSYSCALL* esperarSYSCALL(int socketOrdenes)
{
	int orden;
	recv(socketOrdenes, &orden, sizeof(int), MSG_WAITALL);
	int size;
	recv(socketOrdenes, &size, sizeof(int), MSG_WAITALL);
	int numero;
	recv(socketOrdenes, &numero, sizeof(int), MSG_WAITALL);
	char* texto = malloc(size);
	recv(socketOrdenes, texto, size, MSG_WAITALL);

	void* paqCALL = malloc(size + 3*sizeof(int));
	int desplazamiento = 0;
	memcpy(paqCALL + desplazamiento, &orden, sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(paqCALL + desplazamiento, &size, sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(paqCALL + desplazamiento, &numero, sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(paqCALL + desplazamiento, texto, size);

	return paqCALL;
}

void enviarRespuestaCPU(int socketOrdenes)
{
	send(socketOrdenes, 1, sizeof(int), 0);
}