#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("Uso: pipe2 <archivo> <palabra_par> <palabra_impar>");
		exit(EXIT_FAILURE);
	}

	int pipe1, pipe2, pipe3; 
	int k; 

	/* Archivo con los datos a leer */
	FILE* archivo = fopen(argv[3], "r");

	/* Se generan los pipes */
	pipe(pipe1);
	pipe(pipe2);
	pipe(pipe3);

	while (!feof(archivo)) {
		
	}


}
