#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <readline/readline.h>

#include <setup.h>

int main(void) {
    t_log* logger = log_create("CPU.log", "CPU", 1, LOG_LEVEL_DEBUG);
	log_info(logger, "Iniciando CPU...");

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
		conexionCliente('c','k');
		break;	
	case 'm':
		log_info(logger, "Memoria");
		free(leido);
		conexionCliente('c','m');
		break;
	default:
		log_info(logger, "Comando desconocido");
		free(leido);

		break;
	}
}