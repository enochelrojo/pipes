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

int main(int argc, char** argv) 
{
	if (argc != 4) 
    {
		printf("Uso: pipe2 <archivo> <palabra_par> <palabra_impar>\n");
		exit(EXIT_FAILURE);
	}
	
	int pipe_pares[2], pipe_impares[2], pipe_sort[2]; 
	
	char linea[LARGO_LINEA];
	char lectura[LARGO_LINEA];
	
	pid_t proc_lee, proc_pares, proc_impares;

	/* Archivo con los datos a leer */
	FILE* archivo;


	if ((archivo = fopen(argv[1], "r")) == NULL) 
    {
		printf("Error abriendo %s", argv[1]);
		exit(EXIT_FAILURE);
	}
    
	/* Se generan los pipes */
	pipe(pipe_pares);	
	pipe(pipe_impares);
    pipe(pipe_sort);
         
    int lpares = 1;        
    close(pipe_pares[0]);
    close(pipe_impares[0]);

    proc_lee = fork();

    if (proc_lee == 0) {
        while (fgets(linea, LARGO_LINEA, archivo) != NULL) 
        {
            printf("Linea leida: %s\n", linea);

            if (lpares % 2 == 0) 
            {
                write(pipe_pares[1], linea, LARGO_LINEA);
            } 
            else 
            {
                write(pipe_impares[1], linea, LARGO_LINEA);
            }
            lpares++;
        }

        fclose(archivo);                
    }
    else
    {
        wait(NULL);

        printf("proc padre\n");

        proc_pares = fork();
        if (proc_pares == 0) 
        {
            printf("proc_pares\n");
            // dup2(pipe_pares[0], STDIN_FILENO);          
            // close(pipe_pares[0]);
            // close(pipe_pares[1]);
            
            // dup2(pipe_sort[1], STDOUT_FILENO);
            // close(pipe_sort[0]);
            // close(pipe_sort[1]);

            // execl("/bin/grep", "grep", argv[2], NULL);
            
            close(pipe_pares[1]);

            while(read(pipe_pares[0], lectura, LARGO_LINEA) > 0) {
                printf("Linea: %s", lectura);
            }    
        } 
        else
        {
            proc_impares = fork();

            if (proc_impares == 0) 
            {
                printf("proc_impares\n");
                // dup2(pipe_impares[0], STDIN_FILENO);                
                // close(pipe_impares[0]);
                // close(pipe_impares[1]);
                
                
                // dup2(pipe_sort[1], STDOUT_FILENO);
                // close(pipe_sort[0]);
                // close(pipe_sort[1]);

                // execl("/bin/grep", "grep", argv[3], NULL);
                close(pipe_impares[1]);

                while(read(pipe_impares[0], lectura, LARGO_LINEA) > 0) {
                    printf("Linea: %s", lectura);
                } 
            } 
            else 
            {
                printf("proc_sort\n");
                dup2(pipe_sort[0], STDIN_FILENO);  
                close(pipe_sort[0]);
                close(pipe_sort[1]);
                
                wait(NULL);

                /*while(read(pipe_sort[0], lectura, LARGO_LINEA) > 0) {
                    printf("Linea: %s", lectura);
                }*/
                    
                execl("/usr/bin/sort", "sort", NULL);
                exit(EXIT_SUCCESS);   
            }
        }
    }
}