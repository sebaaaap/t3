compilacion: 

gcc intento4.c -o gg

ejecucion: 

./gg


Instrucciones de Uso
Inicialización
Al iniciar el programa, se te pedirá ingresar los siguientes parámetros:

Tamaño de la memoria física: En megabytes (MB). Ejemplo: 1.
Tamaño de página: En kilobytes (KB). Ejemplo: 256.
El programa calculará automáticamente:

Tamaño de memoria virtual: Igual a dos veces el tamaño de la memoria física.
Tamaño del área de swap: Espacio restante de la memoria virtual después de reservar la memoria física.
Simulación
Durante la ejecución, el programa seguirá estas reglas:

Cada 2 segundos, se crea un nuevo proceso con un tamaño aleatorio entre 1 y 2 páginas.
Cada 5 segundos, a partir de los 10 segundos, se elimina un proceso aleatorio para liberar memoria.
Finalización
El programa finalizará automáticamente si no hay espacio suficiente en RAM ni en el área de swap para asignar nuevas páginas. En este caso, se mostrará un mensaje de error indicando que la memoria está llena.
