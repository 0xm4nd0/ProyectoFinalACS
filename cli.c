#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_LENGTH_COMMAND 30

char** parse_pipe(char* line, int pipe_check);
char** parse_command(char* line);
void exec_command(char** arg_list, int pipe_check);

int main() {
	char line[MAX_LENGTH_COMMAND];
	char *command;
	int pipe_check = 0;
	char **arg_list;

	char *username = getenv("USER");
	
	while(1) {
		printf("%s@proyectoACS: $ ", username);

		if (fgets(line, sizeof line, stdin) != NULL){
			size_t len = strlen(line);
	  		if (len > 0 && line[len-1] == '\n') 
	    		line[--len] = '\0';
		}

		if (strchr(line, '|') != NULL){
			pipe_check = 1;
			arg_list = parse_pipe(line, pipe_check);
			exec_command(arg_list, pipe_check);
		}

		else {
			if (strcmp(line, "exit") == 0)
				break;
			else {
				arg_list = parse_command(line);
				exec_command(arg_list, pipe_check);
			}
		}
	}
	
	free(arg_list);

	return 0;
}

char** parse_pipe(char* line, int pipe_check){
	char **arg_list;
	char *rest = line;

	arg_list = malloc(MAX_LENGTH_COMMAND * sizeof(char*));
	for (int i = 0; i < 2; i++)
		arg_list[i] = malloc((MAX_LENGTH_COMMAND + 1) * sizeof(char));

	arg_list[0] = strtok_r(rest, "|", &rest);
	arg_list[1] = strtok_r(rest, "|", &rest);
	arg_list[2] = NULL;

	//for (int i = 0; i < 2; i++)
		//printf("arg_list[%d]: %s\n", i, arg_list[i]);

	return arg_list;
}

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
		//printf("%s\n", arg_list[num_arguments]);
		//printf("%d\n", num_arguments);
		num_arguments++;
	}

	arg_list[num_arguments] = NULL;
	
	for (int i = 0; i < num_arguments; i++)
		printf("arg_list[%d]: %s\n", i, arg_list[i]);
	
	return arg_list;
}

void exec_command(char** arg_list, int pipe_check){
	pid_t ch_pid;
	
	//If there is no pipe in the input
	if (!pipe_check) {
		printf("No pipe\n");
		ch_pid = fork();
	    if (ch_pid == -1) {
	        perror("fork");
	        exit(EXIT_FAILURE);
	    }

	    //Parent execution
	    if (ch_pid)
	    	wait(NULL);
	    
	    //Child execution
	    else {
	    	//printf("arg_list[0]: %s\n", arg_list[0]);
	        execvp(arg_list[0], arg_list);
	        perror("execvp");
	        exit(EXIT_FAILURE);
	    }

	} 
	
	//If there is a pipe in the input
	else {
		printf("Yes pipe\n");
		//for (int i = 0; i < 2; i++)
			//printf("arg_list[%d]: %s\n", i, arg_list[i]);

		int pipe_fd[2];
		pid_t ch1_pid, ch2_pid;

		char **first_pipe_str;
		char **second_pipe_str;

		pipe(pipe_fd);
		ch1_pid = fork();

		if (ch1_pid == -1) {
	        perror("Fork error in Child");
	        exit(EXIT_FAILURE);
	    }

	    //Child execution
	    if (!ch1_pid){
	    	
			//pipe(pipe_fd);
	    	/*
			ch2_pid = fork();

			if (ch2_pid == -1) {
	        	perror("Fork error in GrandChild");
	        	exit(EXIT_FAILURE);
	    	}

			//Grandchild execution
			if (!ch2_pid) {
			*/
			//close(pipe_fd[1]);
			//close(0);
			//dup(pipe_fd[0]);
			dup2(pipe_fd[0], 0);
			close(pipe_fd[1]);
			second_pipe_str = parse_command(arg_list[1]);
			printf("second_pipe_str: %s\n", second_pipe_str[0]);
			printf("Llegue aqui nieto\n");
			execvp(second_pipe_str[0], second_pipe_str);
			//perror("execvp");
			//exit(EXIT_FAILURE);

			//}
			/*
			//Child execution
			else {
				//close(pipe_fd[0]);
				//close(1);
				//dup(pipe_fd[1]);
				dup2(pipe_fd[1], 1);
				close(pipe_fd[0]);
				first_pipe_str = parse_command(arg_list[0]);
				//printf("first_pipe_str: %s\n", first_pipe_str[0]);
				//printf("Llegue aqui hijo uno\n");
				execvp(first_pipe_str[0], first_pipe_str);
		        //perror("execvp");
		        //exit(EXIT_FAILURE);
			}
			*/
	    }
	    //Parent execution
	    else {
			//wait(NULL);
			//printf("Llegue aqui: Padre (despues de ejecutarse hijo y nieto)\n");
			dup2(pipe_fd[1], 1);
			close(pipe_fd[0]);
			first_pipe_str = parse_command(arg_list[0]);
			//printf("first_pipe_str: %s\n", first_pipe_str[0]);
			//printf("Llegue aqui hijo uno\n");
			execvp(first_pipe_str[0], first_pipe_str);
	        //perror("execvp");
	        //exit(EXIT_FAILURE);
		}

	//free(arg_list);
	free(first_pipe_str);
	free(second_pipe_str);

	}

}

