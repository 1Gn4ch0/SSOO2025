#include <setup.h>

//--------------------------------GLOBAL--------------------------------//

char *selectorModulo(char modulo){
    switch (modulo) {
        case 'i':
            return "IO";
        break;	
        case 'm':
            return "MEMORIA";
        break;
        case 'c':
            return "CPU";
        break;
        default:
            return "KERNEL";
        break;
    }
}

char *selectorPuerto(char moduloA, char moduloB){
    switch (moduloA) {
        case 'i':
            return "4444";
        break;	
        case 'm':
            switch (moduloB) { //memoria
                case 'k':
                    return "1111"; //kernel
                break;	
                default:
                    return "2222"; //cpu
                break;
            }
        break;
        case 'c':
            switch (moduloB) { //cpu
                case 'k':
                    return "3333"; //kernel
                break;	
                default:
                    return "2222"; //memoria
                break;
            }
        break;
        default:
            switch (moduloB) { //kernel
                case 'm':
                    return "1111"; //memoria
                break;	
                default:
                    return "4444"; //io
                break;
            }
        break;
    }
}

char *construirLog(char *a, char *b, char *c)
{
    char *r = string_new();
    string_append(&r, a);
    string_append(&r, b);
    string_append(&r, c);
    return r;
}



//--------------------------------SERVER--------------------------------//

int conexionServidor(char modS, char modC)
{
    char *moduloS = selectorModulo(modS);
    char *moduloC = selectorModulo(modC);
    char *puerto = selectorPuerto(modS, modC);

	t_log *logger = log_create(construirLog(moduloS, ".l", "og"), construirLog(moduloS, "-", moduloC), 1, LOG_LEVEL_DEBUG);

	int server = iniciarServidor(logger, moduloS, moduloC, puerto);
	int socketClient = esperarClient(logger, server, moduloC);

	while (1) {
		int handshake = recibirEntrada(socketClient);
		switch (handshake) {
		case 1:
			log_info(logger, "Mensaje recibido");
			break;
		case -1:
			log_error(logger, "Cliente desconectado, cerrando servidor...");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Horror");
			break;
		}
	}
	return EXIT_SUCCESS;
}

int iniciarServidor(t_log* logger, char *modS, char *modC, char *puerto)
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
	log_info(logger, construirLog(modS, " listo para escuchar al ", modC));

	return socketServidor;
}

int esperarClient(t_log *logger, int socketServidor, char *modC)
{
	int socketClient = accept(socketServidor, NULL, NULL);
	log_info(logger, construirLog(modC, " se conecto", " "));

	return socketClient;
}

int recibirEntrada(int socketClient) //Preventivo
{
	int32_t handshake;
	recv(socketClient, &handshake, sizeof(int32_t), MSG_WAITALL);

	return handshake;
}	



//--------------------------------CLIENT--------------------------------//

void* conexionCliente(char cli, char ser)
{
    char *moduloS = selectorModulo(ser);
    char *moduloC = selectorModulo(cli);
    char *puerto = selectorPuerto(ser, cli);

	t_log *logger = log_create(construirLog(moduloC, ".l", "og"), construirLog(moduloC, "-", moduloS), 1, LOG_LEVEL_DEBUG);

	int conexion;
	char* ip;

	ip = "127.0.0.1";

	// Creamos una conexión hacia el servidor
	conexion = crearConexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviarHandshake(conexion);
	log_info(logger, "Mensaje enviado");

	terminarPrograma(conexion, logger);

}

int crearConexion(char *ip, char* puerto)
{
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviarHandshake(int conexion)
{
	int32_t handshake = 1;
	send(conexion, &handshake, sizeof(int32_t), 0);
}

void terminarPrograma(int conexion, t_log* logger)
{
	int32_t handshake = -1;
	send(conexion, &handshake, sizeof(int32_t), 0);
	close(conexion);
	log_destroy(logger);
}