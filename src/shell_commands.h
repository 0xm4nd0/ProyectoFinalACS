#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

//max number of bytes for command
#define MAX_LENGTH_COMMAND 30

//#define MAX_SIZE 200

//max number of bytes for username
#define USRNAME_SIZE 30

//char** parse_pipe(char* line, int pipe_check);
char** parse_command(char *line);
char* read_command(char *username, char *line);
char* client_read_username();

//PARA EL CLIENTE
char* client_read_username(char *username) {
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


char* read_command(char *username, char *line){
	//char *username = getenv("USER");

	printf("%s@proyectoACS: $ ", username);

	if (fgets(line, MAX_LENGTH_COMMAND, stdin) != NULL){
		size_t len = strlen(line);
  		if (len > 0 && line[len-1] == '\n') 
    		line[--len] = '\0';
	}

	return line;

}

//PARA EL SERVER
char** parse_command(char *line){
	char **arg_list;
	char *rest;
	char *token;
	int num_arguments = 0;
	
	arg_list = malloc(MAX_LENGTH_COMMAND * sizeof(char*));
	for (int i = 0; i < MAX_LENGTH_COMMAND; i++)
		arg_list[i] = malloc((MAX_LENGTH_COMMAND + 1) * sizeof(char));

	rest = line;
	num_arguments = 0;

	while( (token = strtok_r(rest, " ", &rest))) {
		arg_list[num_arguments] = token;
		num_arguments++;
	}

	arg_list[num_arguments] = NULL;
	
	return arg_list;
}

/*
char** parse_pipe(char* line, int pipe_check){
	char **arg_list;
	char *rest = line;

	arg_list = malloc(MAX_LENGTH_COMMAND * sizeof(char*));
	for (int i = 0; i < 2; i++)
		arg_list[i] = malloc((MAX_LENGTH_COMMAND + 1) * sizeof(char));

	arg_list[0] = strtok_r(rest, "|", &rest);
	arg_list[1] = strtok_r(rest, "|", &rest);
	arg_list[2] = NULL;

	return arg_list;
}
*/

