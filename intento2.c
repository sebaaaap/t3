#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>


#define MAX_PROCESSES 100 // Número máximo de procesos

// Estructura de Página
typedef struct {
    int process_id;   // ID del proceso al que pertenece la página
    int page_id;  // Número de página dentro del proceso
    int in_ram;       // 1 si está en RAM, 0 si está en swap
} Pagina;

// Estructura de Proceso
typedef struct {
    int id;     // Identificador del proceso
    int size;   // Tamaño del proceso
    Pagina *pages; // Arreglo de páginas del proceso
} Procesos;

// Estructura de Memoria
typedef struct {
    int id;           // Identificador de la memoria (0 para RAM, 1 para HDD)
    Pagina *pages;    // Arreglo de páginas en memoria
    int num_pages;    // Número de páginas actuales
    int capacity;
    int memgastada;     // Capacidad total de la memoria (en número de páginas)
} Memoria;

// Estructura del Sistema Operativo
typedef struct {
    Procesos *processes[MAX_PROCESSES]; // Array de procesos (punteros)
    int num_processes;  // Número de procesos activos
} So;

// Función para inicializar el sistema
void Iniciarsistema(So *sys) {
    sys->num_processes = 0;  // Inicializa la cantidad de procesos activos
}

// Función para crear un proceso con sus páginas
Procesos *create_process(int id, int size, int page_size) {
    Procesos *p = malloc(sizeof(Procesos));
    if (p == NULL) {
        fprintf(stderr, "Error al asignar memoria para el proceso\n");
        exit(1);
    }
    
    p->id = id;
    p->size = size;

    // Calcular el número de páginas
    int num_pages = size / page_size;
    p->pages = malloc(num_pages * sizeof(Pagina));

    if (p->pages == NULL) {
        fprintf(stderr, "Error al asignar memoria para las páginas\n");
        free(p);
        exit(1);
    }

    // Inicializar páginas con valores aleatorios
    for (int i = 0; i < num_pages; i++) {
        p->pages[i].process_id = id;
        p->pages[i].page_id = i;
        p->pages[i].in_ram = 0;  // Inicialmente, ninguna página está en RAM
    }

    return p;
}

// Función para inicializar la memoria
void inicializar_memoria(Memoria *mem, int id, int capacity) {
    
    mem->id = id;
    mem->capacity = capacity;
    mem->num_pages = 0;
    mem->pages = malloc(capacity * sizeof(Pagina));
    for (int i = 0; i < capacity; i++) {
        mem->pages[i].process_id = -1;  // Indica que la página está vacía
        mem->pages[i].page_id = i;
        mem->pages[i].in_ram = 0;  // Inicialmente, no está en RAM
    }

    printf("Memoria inicializada:\n");
    printf("ID de memoria: %d\n", mem->id);
    printf("Capacidad de memoria: %d\n", mem->capacity);
    printf("Número de páginas: %d\n", mem->num_pages);
    printf("Detalles de las páginas:\n");
}

bool hay_espacio(Memoria *mem, int paginas_necesarias) {
    return (mem->num_pages + paginas_necesarias <= mem->capacity);
}

// Función para alojar las páginas de un proceso en RAM o Swap
void Alojar_en_memoria(Procesos *p, Memoria *ram, Memoria *swap, int sizepagina) {
    int paginas_necesarias = p->size / sizepagina; // Número de páginas necesarias
    int paginas_asignadas = 0;
    int memoria_gastada_en_ram = 0;
    int memoria_gastada_en_swap = 0;

    // Intentar asignar páginas a la RAM
    for (int i = 0; i < paginas_necesarias; i++) {
        if ((ram->num_pages + sizepagina) <= ram->capacity) {
            // Asignar página a RAM
            ram->pages[ram->num_pages].process_id = p->id;
            ram->pages[ram->num_pages].page_id = i;
            ram->pages[ram->num_pages].in_ram = 1; // En RAM
            ram->num_pages += sizepagina; // Incrementar considerando el tamaño de la página

            // Actualizar página del proceso
            p->pages[i].in_ram = 1;

            paginas_asignadas++;
            memoria_gastada_en_ram += sizepagina; // Registrar el tamaño real en RAM
            ram->memgastada = memoria_gastada_en_ram;
        } else if ((swap->num_pages + sizepagina) <= swap->capacity) {
            // No hay espacio en RAM, intentar asignar a Swap
            swap->pages[swap->num_pages].process_id = p->id;
            swap->pages[swap->num_pages].page_id = i;
            swap->pages[swap->num_pages].in_ram = 0; // En Swap
            swap->num_pages += sizepagina; // Incrementar considerando el tamaño de la página

            // Actualizar página del proceso
            p->pages[i].in_ram = 0;

            paginas_asignadas++;
            memoria_gastada_en_swap += sizepagina; // Registrar el tamaño real en Swap
            swap -> memgastada = memoria_gastada_en_swap;
        } else {
            // No hay espacio suficiente ni en RAM ni en Swap
            fprintf(stderr, "Error: No hay espacio suficiente en RAM ni en Swap. Terminando simulación.\n");
            exit(1);
        }
    }

    printf("Proceso ID %d: %d páginas asignadas (Memoria gastada en RAM: %d MB, en Swap: %d MB)\n",
           p->id, paginas_asignadas, memoria_gastada_en_ram , memoria_gastada_en_swap);
}



// Función principal
int main() {
    srand(time(NULL));

    // Parámetros de entrada
    int ram_memory_size = 512;  // Memoria Ram en MB
    int page_size = 4;  // Tamaño de cada página en MB
    int virtual_memory_size = rand() % (ram_memory_size * 4) + (ram_memory_size * 1.5);  // Memoria virtual aleatoria

    So sys;
    Memoria Ram, Virtual;

    // Inicializar el sistema y la memoria
    Iniciarsistema(&sys);
    inicializar_memoria(&Ram, 0, ram_memory_size); // En páginas
    inicializar_memoria(&Virtual, 1, virtual_memory_size); // En páginas

    // Crear procesos cada 2 segundos
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int process_size;

        // Generar un tamaño de proceso que sea múltiplo del tamaño de la página
        do {
            process_size = (rand() % (ram_memory_size / page_size)) * page_size; // Tamaño en múltiplos de page_size
        } while (process_size % page_size != 0 || process_size == 0); // Aseguramos que sea un múltiplo válido

        // Crear el proceso
        Procesos *p = create_process(i, process_size, page_size);

        
        Alojar_en_memoria(p, &Ram, &Virtual, page_size);
       

        // Liberar memoria del proceso
        free(p->pages);
        free(p);

        sleep(2); // Esperar 2 segundos antes de crear el siguiente proceso
    }

    // Liberar la memoria de RAM y Virtual
    free(Ram.pages);
    free(Virtual.pages);

    return 0;
}
