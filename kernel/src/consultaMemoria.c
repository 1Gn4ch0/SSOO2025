#include "consultaMemoria.h"

//orden: 0=iniciar proceso / 1=finalizar proceso / 2=dump memory
int consultaMemoria(t_log* logger, t_config* Kconfig, PaqueteProceso* proceso)
{
    int conexion;
    char* ip = config_get_string_value(Kconfig, "IP_MEMORIA");
    char* puerto = config_get_string_value(Kconfig, "PUERTO_MEMORIA");
    int confirmacion;

	conexion = establecerConexion(ip, puerto);
    enviarConsulta(logger, conexion, proceso);
    confirmacion = esperarConfirmacion(conexion); //0=bien 1=mal

    return confirmacion; 
}

int establecerConexion(char *ip, char* puerto)
{
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, puerto, &hints, &server_info);
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);
	return socket_cliente;
}

void enviarConsulta(t_log* logger, int conexion, PaqueteProceso* proceso)
{
    switch (proceso->orden){
        case 0:
            log_info(logger,"Enviando solicitud de inicio de proceso");
            send(conexion, proceso, sizeof(proceso), 0);
        break;
        case 1:
            log_info(logger,"Enviando mensaje de finalizacion de proceso");
            int finalizacion = 1;
            send(conexion, finalizacion, sizeof(finalizacion), 0);
        break;
        case 2:
            log_info(logger,"Enviando syscall DUMP_MEMORY");
            send(conexion, proceso, sizeof(proceso), 0);
        break;
        default:
            log_error(logger,"ID de consulta a Memoria desconocida");
        break;
    }
}

int esperarConfirmacion(int conexion)
{
    int confirmacion;
    recv(conexion, &confirmacion, sizeof(confirmacion), MSG_WAITALL);
    close(conexion);
    return confirmacion;
}