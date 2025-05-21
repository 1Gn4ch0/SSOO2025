#include "administradorIO.h"

void* administradorIO(void* arg)
{
    paqueteIO* paquete = (paqueteIO*)arg;
    t_log* logger = paquete->logger;
    char* puerto = paquete->puertoIO;
    
	int socketAbierto = abrirConexion(logger, puerto);

    char* dispositivo = "NULL";
    int socketIO = -1;
    while(dispositivo == "NULL" || socketIO == -1){
        socketIO = esperarNuevoDispositivo(logger, socketAbierto);
        dispositivo = identificarDispositivo(logger, socketIO);
    }
    dispositivos[paquete->pos].nombre = dispositivo;
    sem_post(&mutIn2);
    bool conexion = true;

    while(conexion)
    {
        sem_wait(&semDispoI[paquete->pos]);
        enviarOrden(logger,socketIO,paquete->pos);
        conexion = recibirConfirmacion(logger, socketIO, dispositivo);
        sem_post(&semDispoF[paquete->pos]);
    }
    desconectarDispositivo(logger, socketIO, dispositivo, paquete->pos);

}

int abrirConexion(t_log* logger, char *puerto)
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
	log_info(logger, "Socket abierto para nuevo dispositivo");

	return socketServidor;
}

int esperarNuevoDispositivo(t_log* logger, int socketAbierto)
{
    int socketIO = accept(socketAbierto, NULL, NULL);
    if(socketIO == -1){
        log_error(logger, "Fallo al conectar el dispositivo");
    }else{
        log_info(logger, "Dispositivo conectado, esperando identificador");
        return socketIO;
    }
}

char* identificarDispositivo(t_log* logger, int socketIO)
{
    int size;
    int error = recv(socketIO, &size, sizeof(int), MSG_WAITALL);
    char* dispositivo = malloc(size);
    recv(socketIO, dispositivo, size, MSG_WAITALL);
    if(error == -1){
        log_error(logger, "Fallo al identificar el dispositivo");
        return "NULL";
    }else{
        char* logaux = "Dispositivo identificado: ";
        log_info(logger, construirLog(logaux, dispositivo));
        return dispositivo;
    }
}

void enviarOrden(t_log* logger, int socketIO, int pos)
{
    send(socketIO, procesoIO[pos], 3*sizeof(int), 0);
}

bool recibirConfirmacion(t_log* logger, int socketIO, char* dispositivo)
{
    int32_t conf;
    recv(socketIO, &conf, sizeof(int32_t), MSG_WAITALL);
    if(conf==0){
        char* logaux = ": Orden ejecutada exitosamente";
        log_info(logger, construirLog(dispositivo, logaux));
        return true;
    }
    if(conf==1){//error
        char* logaux = ": Error al ejecutar la orden";
        log_info(logger, construirLog(dispositivo, logaux));
        return true;
    }
    if(conf==2){
        char* logaux = ": Desconectando dispositivo";
        log_info(logger, construirLog(dispositivo,logaux));
        return false;
    }
}

void desconectarDispositivo(t_log* logger, int socketIO, char* dispositivo, int pos)
{
    close(socketIO);
    dispositivos[pos].enUso = false;
    sem_post(&contadorDispositivos);
}