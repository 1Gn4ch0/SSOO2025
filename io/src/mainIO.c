#include "mainIO.h"

int main(void) {
    t_config* IOconfig = config_create("io.config");
    config_get_string_value(IOconfig, "LOG_LEVEL");

    char* dispositivo;
	dispositivo = readline(">");

    t_log* logger = log_create("IO.log", construirLog("IO-", dispositivo), 1, LOG_LEVEL_DEBUG);
	log_info(logger, "Iniciando I/O...");

    conexionKernel(dispositivo, logger, IOconfig);

    log_info(logger, "Finalizando I/O...");
    free(dispositivo);
    log_destroy(logger);
	return EXIT_SUCCESS;
}