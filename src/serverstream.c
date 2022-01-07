/*
 ** server.c -- Ejemplo de servidor de sockets de flujo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "shell_commands.h"

#define MYPORT 3490    // Puerto al que conectarán los usuarios

#define BACKLOG 100     // Cuántas conexiones pendientes se mantienen en cola

//#define LINE_MAX 200

#define MAXDATASIZE 5000

//Shell command
void exec_command(char** arg_list, int new_fd);

void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}

int main(int argc, char *argv[])
{
  int sockfd, new_fd, numbytes;  // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
  char user_input[MAXDATASIZE];
  struct sockaddr_in my_addr;    // información sobre mi dirección
  struct sockaddr_in their_addr; // información sobre la dirección del cliente
  int sin_size;
  struct sigaction sa;
  int yes=1;

  //SHELL VARIABLES
  char *username = getenv("USER");
  char **arg_list;
  char *command = (char *) malloc(MAXDATASIZE);
  int server_active = 1;
  char server_exit[10];

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Server-socket() error lol!");
    exit(1);
  }
  else
    printf("Server-socket() sockfd is OK...\n");

  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    perror("Server-setsockopt() error lol!");
    exit(1);
  }
  else
    printf("Server-setsockopt is OK...\n");
        
  my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
  my_addr.sin_port = htons(MYPORT);     // short, Ordenación de bytes de la red
  my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	
  printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);
        
  memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("Server-bind() error");
    exit(1);
  }
  else
    printf("Server-bind() is OK...\n");

  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("Server-listen() error");
    exit(1);
  }
  printf("Server-listen() is OK...Listening...\n");

  sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    perror("Server-sigaction() error");
    exit(1);
  }
  else 
    printf("Server-sigaction() is OK...\n");
  
  printf("\nWaiting for a client connection...\n");

//---------------------INICIA AQUI-----------------
  while(server_active)
  {  // main accept() loop
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
      perror("Server-accept() error");
      continue;
    }
    else
      printf("\nServer-accept() is OK...\n");
    printf("Server-new socket, new_fd is OK...\n");
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));

    // Este es el proceso hijo
    if (!fork()) { 
      close(sockfd); // El hijo no necesita este descriptor

      //-------------SHELL STARTS HERE--------------------
      while(1) {

        //Envío de username
        if (send(new_fd, username, strlen(username), 0) == -1)
          perror("Server-send()");

        //Borrar los elementos del primer buffer recibido del servidor
        //para evitar que se guarden datos innecesarios en próximo envío
        bzero(username, sizeof(username));

        // Como el cliente escribe, yo leo el comando
        //Número de bytes recibidos
        if((numbytes = recv(new_fd, user_input, MAXDATASIZE-1, 0)) == -1)
        {
          perror("recv()");
          exit(1);
        }
        else
          printf("\nCommand received succesfully\n");

        user_input[numbytes] = '\0';

        //Manejo de comando recibido
        command = (char *)realloc(command, strlen(user_input) * sizeof(char));
        strcpy(command, user_input);

        if (strcmp(command, "exit") == 0) {
          printf("Server-Received: %s\n", command);
          printf("Client wants to close the session\n");
          printf("Goodbye to client with fd: %d\n", new_fd);
          break;
        } 
        else {
          printf("Server-Received: %s\n", command);
          arg_list = parse_command(command);
          exec_command(arg_list, new_fd);
        }

      }

      
      free(arg_list);
      free(command);
      printf("\nSe cierra el descriptor del socket cliente y se regresa a esperar otro cliente\n");
      close(new_fd);  // El proceso padre no necesita este descriptor
      printf("Server-new socket, new_fd closed successfully...\n\n");
      printf("Waiting for more clients to connect...\n");
      break;
      
    }

    /*
    printf("Do you want to keep server active? [Yes/No]: ");
    if (fgets(server_exit, sizeof server_exit, stdin) != NULL){
      size_t length = strlen(server_exit);

        if (length > 10) {
          printf("\nInput is too long. Try again\n");
          continue;
        }
        else if (length > 0 && server_exit[length-1] == '\n') 
          server_exit[--length] = '\0';

    }

    if ((strcmp(server_exit, "No") == 0) || (strcmp(server_exit, "N") == 0)) {
      printf("Terminating server...\n");
      printf("Goodbye!\n");
      server_active = 0;
      break;
    }
    else
      if ((strcmp(server_exit, "Yes") == 0) || (strcmp(server_exit, "Y") == 0)) {;
        server_active = 1;
        printf("Waiting for more clients to connect...\n");
        continue;
      } 
    */
    
    //printf("Este es el proceso padre, cierra el descriptor del socket cliente y se regresa a esperar otro cliente\n");
    //close(new_fd);  // El proceso padre no necesita este descriptor
    //printf("Server-new socket, new_fd closed successfully...\n");

  } // Fin del while

  return 0;
}

void exec_command(char** arg_list, int new_fd) {
  pid_t ch_pid;
  int pipe_fd[2], num_bytes;

  pipe(pipe_fd);

  ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //Parent execution
    if (ch_pid) {
      char buffer[MAXDATASIZE];
      close(pipe_fd[1]); //Close the write end of the pipe in the parent

      if ((num_bytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
        buffer[num_bytes] = '\0';
        //pass execvp response buffer to client
        if (send(new_fd, buffer, strlen(buffer), 0) == -1)
          perror("Server-send()");
      }
    }
    //Child execution
    else {
      //Close reading end in the child
      close(pipe_fd[0]);
      dup2(pipe_fd[1], 1); //redirect STDOUT to the writing end of the pipe
      //dup2(pipe_fd[1], 2); //send STDERR to the writing end of the pipe
      close(pipe_fd[1]); //Descriptor no longer needed

        execvp(arg_list[0], arg_list);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
}

void exit_server() {

}