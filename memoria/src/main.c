#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <readline/readline.h>

#include <setup.h>

int main(void) {
	t_log* logger = log_create("MEMORIA.log", "MEMORIA", 1, LOG_LEVEL_DEBUG);
	log_info(logger, "Iniciando Memoria...");
	
	while(true){
		leerComando(logger);
	}

	return EXIT_SUCCESS;
}

void leerComando(t_log* logger)
{
	char* leido;
	leido = readline(">");
	switch (*leido) {
	case 'k':
		log_info(logger, "Kernel");
		free(leido);
		conexionServer('m','k');
		break;	
	case 'c':
		log_info(logger, "CPU");
		free(leido);
		conexionServer('m','c');
		break;
	default:
		log_info(logger, "Comando desconocido");
		free(leido);

		break;
	}
}