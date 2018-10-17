#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv) {
	if (argc != 5) {
		printf("Uso: pipe <palabra> <archivo1> <archivo2> <archivo3>");
		exit(EXIT_FAILURE);
	}

	pid_t proc1, proc2;
	int salida;

	/* Pipe: fd[0] para lectura - fd[1] para escritura */
	int fd[2];

	if (pipe(fd) == -1) {
		printf("Error: al crear el pipe");
		exit(EXIT_FAILURE);
	}

	if ((proc1 = fork()) == -1) {
		printf("Error: al crear proceso 1");
		exit(EXIT_FAILURE);
	}

	/* Redirecciono la salida estandar al pipe */

	if (proc1 == 0) {
		/* Proceso 1: lee las lineas que contengan la palabra pasada por parametro, 
			desde el primer archivo, y las escribe en el pipe */
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		close(fd[0]);

		execl("/bin/grep", "grep", argv[1], argv[2], NULL);
	} else {
		if ((proc2 = fork()) == -1) {
			printf("Error: al crear proceso 2");
			exit(EXIT_FAILURE);
		}

		if (proc2 == 0) {
			/* Proceso 2: lee las lineas que contengan la palabra pasada por parametro, 
				desde el segundo archivo, y las escribe en el pipe */
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
			close(fd[0]);
			execl("/bin/grep", "grep", argv[1], argv[3], NULL);
		} else {
			dup2(fd[0], STDIN_FILENO);
			close(fd[1]);
			close(fd[0]);

			salida = creat(argv[4], 0644);

			dup2(salida, STDOUT_FILENO);
			close(salida);

			execl("/usr/bin/wc", "wc", "-l", NULL);
		}

		return 0;
	}
}
