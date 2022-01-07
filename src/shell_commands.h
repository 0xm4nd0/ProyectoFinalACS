#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_LENGTH_COMMAND 30
#define MAX_SIZE 300

//char** parse_pipe(char* line, int pipe_check);
char** parse_command(char* line);
void read_command(char* username, char *line);

//PARA EL CLIENTE
void read_command(char *username, char *line){
	
	//char *username = getenv("USER");

	printf("%s@proyectoACS: $ ", username);
	fflush(stdout);

	if (fgets(line, sizeof line, stdin) != NULL){
		size_t len = strlen(line);
  		if (len > 0 && line[len-1] == '\n') 
    		line[--len] = '\0';
	}

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

