#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
    int capacity;     // Capacidad total de la memoria (en número de páginas)
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
    inicializar_memoria(&Ram, 0, ram_memory_size / page_size); // En páginas
    inicializar_memoria(&Virtual, 1, virtual_memory_size / page_size); // En páginas

    // Crear procesos cada 2 segundos
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int process_size;

        // Generar un tamaño de proceso que sea múltiplo del tamaño de la página
        do {
            process_size = (rand() % (ram_memory_size / page_size)) * page_size; // Tamaño en múltiplos de page_size
        } while (process_size % page_size != 0 || process_size == 0); // Aseguramos que sea un múltiplo válido

        // Crear el proceso
        Procesos *p = create_process(i, process_size, page_size);

        // Imprimir información del proceso y sus páginas
        printf("Proceso ID: %d, Tamaño: %d MB, Número de páginas: %d\n", 
               p->id, p->size, p->size / page_size);
        
        for (int j = 0; j < p->size / page_size; j++) {
            printf("  Página %d del Proceso %d, en RAM: %d\n", 
                   p->pages[j].page_id, p->id, p->pages[j].in_ram);
        }

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
