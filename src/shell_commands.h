/*
  Alumnos: Espino Rojas Hector Daniel
           Vargas Guerrero Armando

  Grupo: 01

  Objetivo: 
    Creación de un modelo cliente-servidor que ejecuta
    comandos remotamente, como en la implementación de 
    SSH.

  En esta biblioteca se definen algunas funciones adicionales
  para los programas del cliente y el servidor.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

//Numéro máximo de bytes permitidos para el ingreso de comando
#define MAX_LENGTH_COMMAND 30

//Número máximo de bytes permitidos para el ingreso de usuario
#define USRNAME_SIZE 30

char** parse_command(char *line);
char* read_command(char *username, char *line);
char* client_read_username();

//Funciones del lado del cliente:

//En esta función se lee el nombre de usuario asignado por el cliente
char* client_read_username(char *username) {
	
	//Bandera que determina si se recibió un nombre de usuario válido	
	int valid_username = 0;

	while(!valid_username) {
		printf("\nIngrese un nombre de usuario: ");

		if (fgets(username, USRNAME_SIZE, stdin) != NULL){
			size_t len = strlen(username);

			if (*username == '\n')
				continue;
	  		else if (len > 0 && username[len-1] == '\n') {
	    		username[--len] = '\0';
	    		valid_username = 1;

	  		}
	  		else continue;
		}

	}
	printf("\n");
	return username;

}

//En esta función se lee toda la entrada (el comando)
//que el cliente desea ingresar
char* read_command(char *username, char *line){

	//Se imprime el nombre de usuario y el nombre del host
	//como sucede al conectarte por SSH
	printf("%s@proyectoACS: $ ", username);

	//Se lee la entrada del cliente
	if (fgets(line, MAX_LENGTH_COMMAND, stdin) != NULL){
		size_t len = strlen(line);
  		if (len > 0 && line[len-1] == '\n') 
    		line[--len] = '\0';
	}

	return line;

}

//Función para el servidor

/*
	En esta función se procesa (parsea) la entrada del usuario.
	Se separan los argumentos que se hayan detectado en el comando ingresado
	y se almacenan en un arreglo.
*/
char** parse_command(char *line){
	char **arg_list; //Arreglo de argumentos
	char *rest; //Apuntador auxiliar que hace referencia a la entrada del cliente
	char *token; //El token es el argumento detectado tras un " "
	int num_arguments = 0; //Número de argumentos detectados
	
	//Asignación de memoria en el arreglo de argumentos
	arg_list = malloc(MAX_LENGTH_COMMAND * sizeof(char*));
	for (int i = 0; i < MAX_LENGTH_COMMAND; i++)
		arg_list[i] = malloc((MAX_LENGTH_COMMAND + 1) * sizeof(char));

	rest = line;
	num_arguments = 0;

	//Se asignan los argumentos (tokens) detectados en el arreglo
	while( (token = strtok_r(rest, " ", &rest))) {
		arg_list[num_arguments] = token;
		num_arguments++;
	}

	//Se declara el fin de la asignación de argumentos
	arg_list[num_arguments] = NULL;
	
	return arg_list;
}


