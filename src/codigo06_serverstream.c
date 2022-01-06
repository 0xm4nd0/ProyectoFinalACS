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

#define LINE_MAX 200

#define MAXDATASIZE 300

void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}

int main(int argc, char *argv[])
{
  int sockfd, new_fd, numbytes;  // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
  char buf[MAXDATASIZE];
  struct sockaddr_in my_addr;    // información sobre mi dirección
  struct sockaddr_in their_addr; // información sobre la dirección del cliente
  int sin_size;
  struct sigaction sa;
  int yes=1;

  //SHELL VARIABLES
  char *username = getenv("USER");


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

//---------------------INICIA AQUI-----------------
  while(1)
  {  // main accept() loop
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
    {
      perror("Server-accept() error");
      continue;
    }
    else
      printf("Server-accept() is OK...\n");
    printf("Server-new socket, new_fd is OK...\n");
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));

    // Este es el proceso hijo
    if (!fork()) { 
      close(sockfd); // El hijo no necesita este descriptor

      while(1) {

      }


      // Como el cliente escribe, yo leo el mensaje

      //Número de bytes recibidos
      if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1)
      {
        perror("recv()");
        exit(1);
      }
      else
        printf("Servidor-The recv() is OK...\n");

      buf[numbytes] = '\0';
      printf("Servidor-Received: %s", buf);

      



      // Ahora yo capturo del teclado para responder al cliente
      printf("Escribe un mensaje a enviar\n");
      char linea1[LINE_MAX]; // podemos usarlo por el fgets
      fgets(linea1,LINE_MAX,stdin);
      printf("El mensaje a enviar es: %s", linea1);
      if (send(new_fd, linea1, strlen(linea1), 0) == -1)
        perror("Server-send() error lol!");
      close(new_fd);
      exit(0);
    }
    printf("Este es el proceso padre, cierra el descriptor del socket cliente y se regresa a esperar otro cliente\n");
    close(new_fd);  // El proceso padre no necesita este descriptor
    printf("Server-new socket, new_fd closed successfully...\n");
  } // Fin del while

  return 0;
}
