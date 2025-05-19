#include <auxFunctions.h>

char* construirLog(char *a, char *b)
{
    char *r = string_new();
    string_append(&r, a);
    string_append(&r, b);
    return r;
}

char* ajustarPuerto(char* puerto, int ajuste)
{
	int num = atoi(puerto);
	num = num + ajuste;
	char* r = string_itoa(num);
	return r;
}