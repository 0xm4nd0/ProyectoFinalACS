/*
  Alumnos: Espino Rojas Hector Daniel
           Vargas Guerrero Armando

  Grupo: 01

  Objetivo: 
    Creación de un modelo cliente-servidor que ejecuta
    comandos remotamente, como en la implementación de 
    SSH.

  Éste es el programa correspondiente al servidor.

*/

//Inclusión de bibliotecas predefinidas

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

//Inclusión de funciones propias
#include "shell_commands.h"

// Puerto al que se conectará el cliente
#define MYPORT 3490    

// Cuántas conexiones pendientes se mantienen en cola
#define BACKLOG 100     

//Cantidad máxima de bytes que se envían en el socket
#define MAXDATASIZE 11000 

//Prototipo de función donde se ejecutarán los comandos recibidos
void exec_command(char** arg_list, int new_fd);

//Función principal que inicializa al servidor
int main(int argc, char *argv[])
{
  // Escuchar sobre sock_fd | nuevas conexiones sobre new_fd | número de bytes recibidos del cliente
  int sockfd, new_fd, numbytes;  

  // información sobre la dirección del servidor
  struct sockaddr_in my_addr;    

  // información sobre la dirección del cliente
  struct sockaddr_in their_addr; 
  int sin_size;
  int yes=1;

  //Buffer que almacena toda la entrada ingresada por el usuario
  char user_input[MAXDATASIZE];

  //Arreglo que almacena los argumentos ingresados por el usuario
  char **arg_list;

  //Comando que se registró de acuerdo a la entrada del usuario
  char *command = (char *) malloc(MAXDATASIZE);

  //Banderas que determinan si el servidor seguirá ejecutándose o no
  int server_active = 1;
  int valid_response = 0;

  //Buffer que almacena la respuesta del administrador del servidor
  //respecto a la ejecución del servidor
  char server_exit[4];

  //Se crea un nuevo socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Server-socket() error lol!");
    exit(1);
  }
  else
    printf("Server-socket() sockfd is OK...\n");

  //Permitir el reuso de direcciones locales
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    perror("Server-setsockopt() error lol!");
    exit(1);
  }
  else
    printf("Server-setsockopt OK...\n");
        
  my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
  my_addr.sin_port = htons(MYPORT);     // short, Ordenación de bytes de la red
  my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	
  printf("Servidor utiliza la direccion %s y el puerto %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);
  
  // Se asigna un byte nulo en el resto de la estructura
  memset(&(my_addr.sin_zero), '\0', 8); 

  //Se asigna la dirección especificada en el socket referido en el File Descriptor
  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("Server-bind() error");
    exit(1);
  }
  else
    printf("Server-bind() is OK...\n");

  //Se pone el servidor a la escucha
  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("Server-listen() error");
    exit(1);
  }
  printf("Server-listen() OK...\n");
  
  printf("\nEn espera de la conexion de un cliente...\n");

//---------------------Inicio de ejecución de servidor-------------------

  //El servidor se mantendrá activo de acuerdo a la bandera que se lo indica
  while(server_active)
  {  
    //Se determina si se acepta la conexión de un cliente
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
      perror("Server-accept() error");
      continue;
    }
    else
      printf("\nServer-accept() OK...\n");
    printf("Server-new socket -> new_fd (cliente) OK...\n");
    printf("Server: Se obtuvo conexion de %s\n", inet_ntoa(their_addr.sin_addr));

    valid_response = 0;

      //-------------Inicio de recepción de comandos del cliente--------------------
      while(1) {

        // Se determina el número de bytes correspondientes a la entrada recibida del cliente
        if((numbytes = recv(new_fd, user_input, MAXDATASIZE-1, 0)) == -1)
        {
          perror("recv()");
          exit(1);
        }
        else
          printf("\nEl comando se recibio de forma exitosa\n");

        user_input[numbytes] = '\0';

        //Se almacena el comando recbido de la entrada
        command = (char *)realloc(command, strlen(user_input) * sizeof(char));
        strcpy(command, user_input);
        printf("El servidor recibio el siguiente comando: %s\n", command);

        //En caso de que se registre "exit", se termina la conexión con el cliente
        if (strcmp(command, "exit") == 0) {
          printf("\nEl cliente quiere terminar la conexion\n");
          printf("Adios al cliente con el siguiente file_descriptor: %d\n", new_fd);
          break;
        } 
        //En caso contrario, se pasa a la ejecución del comando registrado
        else {      
          arg_list = parse_command(command);
          exec_command(arg_list, new_fd);
        }

      } //Fin de la recepción de comandos

      printf("\nSe cierra el descriptor del socket cliente\n");
      close(new_fd);  // El proceso padre no necesita este descriptor
      printf("Server-new socket, el fd del cliente se cerro exitosamentente...\n");

      //En caso de que el cliente haya terminado la conexión, se pregunta si se
      //desea mantener activo el servidor
      while (!valid_response) {
        printf("\nSe desea mantener el servidor activo? [Si/No]: ");
        if (fgets(server_exit, sizeof server_exit, stdin) != NULL){
          size_t length = strlen(server_exit);

            if (length > strlen("Si ")) {
              printf("La respuesta es muy extensa. Intente nuevamente\n");
              continue;
            }
            else if (length > 0 && server_exit[length-1] == '\n') 
              server_exit[--length] = '\0';
              
            if ((strcmp(server_exit, "No") == 0) || (strcmp(server_exit, "N") == 0) || 
                (strcmp(server_exit, "Si") == 0) || (strcmp(server_exit, "S") == 0)) {
              valid_response = 1;
            }
              else
                printf("Respuesta invalida. Intente nuevamente.\n");
            
        }

      }

      if ((strcmp(server_exit, "No") == 0) || (strcmp(server_exit, "N") == 0)) {
        printf("\nTerminando servidor...\n");
        printf("Hasta luego!\n\n");
        server_active = 0;
        break;

      }
      else
        if ((strcmp(server_exit, "Si") == 0) || (strcmp(server_exit, "S") == 0)) {
          printf("\nEn espera de la conexion de otro cliente...\n");
        } 
      

  } // Fin de la ejecución del servidor

  free(arg_list);
  free(command);
  close(sockfd);
  return 0;
}

//Esta función va a ejecutar el comando ingresado por el cliente
//de acuerdo a la lista de parámetros recibidos
void exec_command(char** arg_list, int new_fd) {
  pid_t ch_pid;
  int pipe_fd[2], num_bytes;

  //La ejecución del comando se debe realizar por medio de la comunicación
  //por un pipe entre un proceso padre y un proceso hijo 
  pipe(pipe_fd);

  ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //El proceso padre va a recibir la salida de la función execvp del proceso hijo
    if (ch_pid) {
      //Buffer que almacenará el resultado obtenido por el proceso hijo
      char exec_result[MAXDATASIZE];
      
      //Se cierra el modo de escritura del pipe, del lado del padre
      close(pipe_fd[1]); 

      //Se registra el número de bytes correspondientes a la salida de execvp
      //y se añade el fin de cadena después del último byte recibido
      if ((num_bytes = read(pipe_fd[0], exec_result, sizeof(exec_result))) > 0) {
        exec_result[num_bytes] = '\0';

        //Se pasa al cliente el buffer con la respuesta de execvp
        if (send(new_fd, exec_result, strlen(exec_result), 0) == -1)
          perror("Server-send()");
      }
    }

    //El proceso hijo va a procesar la lista de argumentos del comando
    //mediante execvp
    else {

      close(pipe_fd[0]); //Se cierra el modo de lectura del pipe, del lado del hijo
      dup2(pipe_fd[1], 1); //Se redirige STDOUT al modo de escritura del pipe
      dup2(pipe_fd[1], 2); //Se redirige STDERR al modo de escritura del pipe
      close(pipe_fd[1]); //Se cierra el modo de escritura del pipe, del lado del hijo

        execvp(arg_list[0], arg_list);

        //En caso de que no se realice correctamente la ejecución
        perror("execvp");
        exit(EXIT_FAILURE);
    }
}
