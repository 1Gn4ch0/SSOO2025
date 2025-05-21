#include <conexionKernel.h>

int conexionKernel(char* dispositivo, t_log* logger, t_config* IOconfig)
{
	int conexion;
	char* ip = config_get_string_value(IOconfig, "IP_KERNEL");
	char* puerto = config_get_string_value(IOconfig, "PUERTO_KERNEL");

	conexion = conectarDispositivo(ip, puerto); //Creamos una conexión hacia el kernel

	enviarHandshakeIO(conexion, dispositivo);
	log_info(logger, "Handshake enviado");

	int cicloDeOrden = 1;
	while(cicloDeOrden == 1)
	{ //el kernel mandara una struct con la orden, tiempo de ejecucion e PID
		paqueteOrden* orden = esperarRespuestaIO(conexion, logger);
		cicloDeOrden = ejecutarOrdenIO(conexion, logger, orden);
	}

	return EXIT_SUCCESS;
}

int conectarDispositivo(char *ip, char* puerto)
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

void enviarHandshakeIO(int conexion, char* dispositivo)
{
	handshake* hs = malloc(sizeof(handshake));
	hs->size = strlen(dispositivo)+1;
	hs->nombre = malloc(hs->size);
	memcpy(hs->nombre, dispositivo, hs->size);

	//Extraido del tp0
	void* magic = malloc(hs->size + sizeof(int));
	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(hs->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, hs->nombre, hs->size);
	desplazamiento+= hs->size;

	send(conexion, magic, hs->size + sizeof(int), 0);
}

paqueteOrden* esperarRespuestaIO(int conexion, t_log* logger)
{
	paqueteOrden* orden = malloc(3*sizeof(int));
	orden->orden = -1;
	while(orden->orden < 1){
		recv(conexion, &orden, 3*sizeof(int), MSG_WAITALL);
		if(orden->orden < 1){
			log_info(logger, "Orden desconocida");
		}
	}
	return orden;
}

int ejecutarOrdenIO(int conexion, t_log* logger, paqueteOrden* orden)
{
	switch (orden->orden){
		case 0:
			log_info(logger, "Finalizando Conexion");
			desconectarDispositivo(conexion, logger);
			return 1;
		break;
		case 1:
			log_info(logger, "Escritura");
			confirmacionDeOrden(conexion, 0);
			return 0;
		break;
		case 2:
			log_info(logger, "Lectura");
			confirmacionDeOrden(conexion, 0);
			return 0;
		break;
		default:
			log_info(logger, "ID de orden desconocida");
			confirmacionDeOrden(conexion, 1);
			return 0;
		break;
	}
}
void confirmacionDeOrden(int conexion, int32_t confirmacion)
{
	send(conexion, confirmacion, sizeof(int32_t), 0);
}

void desconectarDispositivo(int conexion, t_log* logger)
{
	int32_t avisoDeCierre = 2;
	send(conexion, &avisoDeCierre, sizeof(int32_t), 0);
	close(conexion);
}
