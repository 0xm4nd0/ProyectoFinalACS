# Proyecto Final Arquitectura Cliente-Servidor

## Objetivo
En este proyecto se encuentra una implementación de un modelo Cliente-Servidor que ejecuta comandos remotamente, como ocurre con el protocolo SSH.

![plot](./ClientServerExample.png)

## Ejecución
Para ejecutarlo, de manera **local**, nos ubicamos dentro de la carpeta `src` y realizamos lo siguiente:

1. Se compila el archivo `serverstream.c`
    ```console
        gcc clientstream.c -o serverstream
    ```
2. Se compila el archivo `clientstream.c`
    ```console
        gcc clientstream.c -o clientstream
    ```
3. Ejecutamos el programa `serverstream`
    ```console
        ./severstream
    ```
4. Por último, ejecutamos el programa `clientstream` con el parámetro de `localhost`
    ```console
        ./clientstream localhost
    ```
Para finalizar la conexión del lado del cliente, ingresamos el comando `exit`.

Cuando se finalice la conexión con el cliente, del lado del servidor se preguntará si se desea que éste se mantenga activo o no.

## Resultados
* Del lado del servidor:

![plot](./serverExecution.png)

* Del lado del cliente: 

![plot](./clientExecution.png)
