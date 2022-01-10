#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "shell_commands.h"

// the port client will be connecting to
#define PORT 3490
// max number of bytes we can get at once
#define MAXDATASIZE 5000

//max number of bytes of command
#define LINE_MAX 30

//max number of bytes for username
#define USRNAME_SIZE 30

int main(int argc, char *argv[])
{
  int sockfd, usr_bytes, exec_bytes;
  struct hostent *he;

  //Respuesta del servidor
  char server_usr[MAXDATASIZE];
  char server_execution[MAXDATASIZE];
  
  //shell variables
  char *command = (char *) malloc(sizeof (char) * LINE_MAX); // podemos usarlo por el fgets
  //char *username = (char *) malloc(sizeof (char) * MAXDATASIZE);
  char* username = (char *) malloc (USRNAME_SIZE * sizeof (char));
  //char *result = (char *) malloc(MAXDATASIZE);

  // connector’s address information
  struct sockaddr_in their_addr;

  // if no command line argument supplied
  if(argc != 2)
  {
    fprintf(stderr, "Client-Usage: %s hostname_del_servidor\n", argv[0]);
    // just exit
    exit(1);
  }

  // get the host info
  if((he=gethostbyname(argv[1])) == NULL)
  {
    perror("gethostbyname()");
    exit(1);
  }
  else
    printf("Client-The remote host is: %s\n", argv[1]);

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket()");
    exit(1);
  }
  else
    printf("Client-The socket() sockfd is OK...\n");


  // host byte order
  their_addr.sin_family = AF_INET;
  // short, network byte order
  printf("Server-Using %s and port %d...\n", argv[1], PORT);
  their_addr.sin_port = htons(PORT);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  // zero the rest of the struct
  memset(&(their_addr.sin_zero), '\0', 8);

  if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("connect()");
    exit(1);
  }
  else
    printf("Client-The connect() is OK...\n");


  //---------------SHELL STARTS HERE----------------------
  
  /*
  // Se va a recibir el username del servidor
  if ((usr_bytes = recv(sockfd, server_usr, MAXDATASIZE-1, 0)) == -1) {
    perror("Server-recv() fail");
    exit(1);
  }
  else {
    //Usuario obtenido
    server_usr[usr_bytes] = '\0';
    username = (char *)realloc(username, strlen(server_usr) * sizeof(char));
    strcpy(username, server_usr);

  }

  */

  username = client_read_username(username);

  while(1) {
    
    //Se lee comando del usuario
    command = read_command(username, command);

    if (strlen(command) < 1)
      continue; 
      
    if (send(sockfd, command, strlen(command), 0) == -1)
      perror("Client-send() fail");

    if (strcmp(command, "exit") == 0) {
      printf("\nConnection terminated...\n");
      break;
    }

    //Se recibe respuesta del servidor   
    if ((exec_bytes = recv(sockfd, server_execution, MAXDATASIZE-1, 0)) == -1) {
      perror("Server-recv() fail");
      exit(1);
    }
    else {
      //Respuesta de execvp obtenida
      server_execution[exec_bytes] = '\0';

      printf("%s\n", server_execution);
      fflush(stdout);
      server_execution[0] = '\0';

    }

  }
 
  printf("Client-Closing sockfd\n");
  printf("Goodbye!\n");
  close(sockfd);
  free(command);
  free(username);
  return 0;
}

