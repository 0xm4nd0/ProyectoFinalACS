/*
  Alumnos: Espino Rojas Hector Daniel
           Vargas Guerrero Armando

  Grupo: 01

  Objetivo: 
    Creación de un modelo cliente-servidor que ejecuta
    comandos remotamente, como en la implementación de 
    SSH.

  Éste es el programa correspondiente al cliente.

*/

//Inclusión de bibliotecas predefinidas

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

//Cantidad máxima de bytes que se envían en el socket
#define MAXDATASIZE 11000

//Numéro máximo de bytes permitidos para el ingreso de comando
#define LINE_MAX 30

//Número máximo de bytes permitidos para el ingreso de usuario
#define USRNAME_SIZE 30

//Función principal que inicializa al cliente
int main(int argc, char *argv[])
{
  int sockfd, usr_bytes, exec_bytes;
  struct hostent *he;
  int PORT;

  //En este buffer se almacena la respuesta producida por el servidor
  char server_execution[MAXDATASIZE];
  
  //Apuntadores a cadena que almacenarán el nombre de usuario y el comando ingresado
  char *command = (char *) malloc(sizeof (char) * LINE_MAX); 
  char* username = (char *) malloc (USRNAME_SIZE * sizeof (char));

  // información sobre la dirección del cliente
  struct sockaddr_in their_addr;

  // En el caso de que no se ingresen los parámetros necesarios en la línea de comandos
  if(argc != 3)
  {
    fprintf(stderr, 
      "La ejecucion del cliente es la siguiente: \n\t%s hostname_del_servidor puerto_servidor\n", argv[0]);
    
    // Se finaliza la ejecución del cliente
    exit(1);
  } else 
      PORT = atoi(argv[2]);

  // Se obtiene la información del host del servidor
  if((he=gethostbyname(argv[1])) == NULL)
  {
    perror("gethostbyname()");
    exit(1);
  }
  else
    printf("\nCliente -> El host remoto es: %s\n", argv[1]);

  //Se crea un nuevo socket
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket()");
    exit(1);
  }
  else
    printf("Cliente-socket() -> sockfd OK...\n");


  //Orden de bytes del host
  their_addr.sin_family = AF_INET;

  //Orden de bytes para la red
  printf("El servidor esta usando la direccion %s y el puerto %d...\n", argv[1], PORT);
  their_addr.sin_port = htons(PORT);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  
  // // Se asigna un byte nulo en el resto de la estructura
  memset(&(their_addr.sin_zero), '\0', 8);

  if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("connect()");
    exit(1);
  }
  else
    printf("Cliente -> connect() OK...\n");

  //Cuando la conexión es exitosa, se le pide al cliente que ingrese un nombre de usuario
  //(que se almacenará solamente del lado del cliente)

  username = client_read_username(username);

  //---------------Inicio de ingreso de comandos del cliente-----------------------
  while(1) {
    
    //Se lee comando del usuario
    command = read_command(username, command);

    //Se omite que el usuario haya ingresado un salto de línea
    if (strlen(command) < 1)
      continue; 
    
    //Se detecta si el comando ingresado se envía correctamente
    if (send(sockfd, command, strlen(command), 0) == -1)
      perror("Client-send() fail");

    //En caso de que se ingrese "exit", se termina la conexión del cliente
    if (strcmp(command, "exit") == 0) {
      printf("\nTerminando conexion...\n");
      break;
    }

    //Se recibe el número de bytes de la respuesta de la ejecución del servidor   
    if ((exec_bytes = recv(sockfd, server_execution, MAXDATASIZE-1, 0)) == -1) {
      perror("Server-recv() fail");
      exit(1);
    }
    else {
      //En el caso de que la recepción sea exitosa
      //se lee la respuesta de la función execvp

      //Se agrega un fin de cadena después del último byte recibido del servidor
      server_execution[exec_bytes] = '\0';

      //Se imprimen los resultados
      printf("%s\n", server_execution);
      fflush(stdout);
      //server_execution[0] = '\0';

    }

  }
 
  printf("Cliente -> Se cierra sockfd\n");
  printf("¡Hasta luego!\n\n");
  close(sockfd);
  free(command);
  free(username);
  return 0;
}

