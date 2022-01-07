# Proyecto Final Arquitectura Cliente-Servidor
En este proyecto se encuentra una implementación de un modelo Cliente-Servidor que ejecuta comandos remotamente, como ocurre con el protocolo SSH.

Para ejecutarlo, de manera local, nos ubicamos dentro de la carpeta `src` y realizamos lo siguiente:

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
