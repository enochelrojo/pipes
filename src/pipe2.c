#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define LARGO_LINEA 255

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("Uso: pipe2 <archivo> <palabra_par> <palabra_impar>\n");
		exit(EXIT_FAILURE);
	}

	
	int pipe_pares[2], pipe_impares[2], pipe_sort[2]; 
	int k = 0; 
	char linea[LARGO_LINEA];
	char lectura[LARGO_LINEA];
	
	pid_t proc_lee, proc_pares, proc_impares;


	/* Archivo con los datos a leer */
	FILE* archivo;


	if ((archivo = fopen(argv[1], "r")) == NULL) {
		printf("Error abriendo %s", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Se generan los pipes */
	if(pipe(pipe_pares) == -1) {
		printf("Error abriendo pipe pares");
		exit(EXIT_FAILURE);
	}
	
	if(pipe(pipe_impares) == -1) {
		printf("Error abriendo pipe impares");
		exit(EXIT_FAILURE);			
	}


	if ((proc_lee = fork()) == 0) {
		printf("Leemos\n");

		int cpares = 1;
		close(pipe_pares[0]);
		close(pipe_impares[0]);

		while (fgets(linea, LARGO_LINEA, archivo) != NULL) {
			if (cpares % 2 == 0) {
				write(pipe_pares[1], linea, LARGO_LINEA);
			} else {
				write(pipe_impares[1], linea, LARGO_LINEA);
			}
			cpares++;
		}
	} else {		
		if(pipe(pipe_sort) == -1) {
				printf("Error abriendo pipe para sort");
				exit(EXIT_FAILURE);
		}

		if ((proc_pares = fork()) == 0) {
			printf("Pares\n");
			dup2(pipe_pares[0], STDIN_FILENO);
			close(pipe_pares[0]);
			close(pipe_pares[1]);

			dup2(pipe_sort[1], STDOUT_FILENO);
			close(pipe_sort[0]);
			close(pipe_sort[1]);

			execl("/bin/grep", "grep", argv[2], NULL);

			exit(EXIT_SUCCESS);			
		} else {
			if ((proc_impares = fork()) == 0) {
				printf("ImPares\n");
				dup2(pipe_impares[0], STDIN_FILENO);
				

				close(pipe_impares[0]);
				close(pipe_impares[1]);

				dup2(pipe_sort[1], STDOUT_FILENO);
				close(pipe_sort[0]);
				close(pipe_sort[1]);

				execl("/bin/grep", "grep", argv[3], NULL);
				exit(EXIT_SUCCESS);
			}
			else
			{
				wait(NULL);
				printf("Sort\n");
				//dup2(pipe_sort[0], STDIN_FILENO);

				while(read(pipe_sort[0], lectura, LARGO_LINEA) > 0) {
					printf("%s\n", lectura);
				}
/*
				close(pipe_sort[0]);
				close(pipe_sort[1]);
				*/printf("Holis");/*
				execl("/usr/bin/sort", "sort", NULL);
				exit(0);*/
			}			
		}
		
	}
	
	fclose(archivo);
	exit(EXIT_SUCCESS);
}