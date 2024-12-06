#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_PROCESSES 100 // Número máximo de procesos
#define MAX_THREADS 10    // Máximo número de hilos simultáneos

// Estructura de Página
typedef struct {
    int process_id; // ID del proceso al que pertenece la página
    int page_id;    // Número de página dentro del proceso
    int in_ram;     // 1 si está en RAM, 0 si está en swap
    int size;
} Pagina;

// Estructura de Proceso
typedef struct {
    int id;        // Identificador del proceso
    int size;      // Tamaño del proceso
    Pagina *pages; // Arreglo de páginas del proceso
} Procesos;

// Estructura de Memoria
typedef struct {
    int id;          // Identificador de la memoria (0 para RAM, 1 para HDD)
    Pagina *pages;   // Arreglo de páginas en memoria
    int num_pages;   // Número de páginas actuales
    int capacity;    // Capacidad total de la memoria (en número de páginas)
    int memgastada;  // Memoria utilizada
} Memoria;

// Mutex para sincronización
pthread_mutex_t memory_mutex;

// Inicializar la memoria
void inicializar_memoria(Memoria *mem, int id, int capacity) {
    mem->id = id;
    mem->capacity = capacity;
    mem->num_pages = 0;
    mem->memgastada = 0;
    mem->pages = malloc(capacity * sizeof(Pagina));
    for (int i = 0; i < capacity; i++) {
        mem->pages[i].process_id = -1; // Página vacía
        mem->pages[i].page_id = i;
        mem->pages[i].in_ram = 0;
    }
}

// Función para crear un proceso con sus páginas
Procesos *create_process(int id, int size, int page_size) {
    Procesos *p = malloc(sizeof(Procesos));
    if (!p) {
        fprintf(stderr, "Error al asignar memoria para el proceso\n");
        exit(1);
    }
    p->id = id;
    p->size = size;

    int num_pages = size / page_size;
    p->pages = malloc(num_pages * sizeof(Pagina));
    if (!p->pages) {
        fprintf(stderr, "Error al asignar memoria para las páginas\n");
        free(p);
        exit(1);
    }
    for (int i = 0; i < num_pages; i++) {
        p->pages[i].process_id = id;
        p->pages[i].page_id = i;
        p->pages[i].in_ram = 0; // Inicialmente no están en RAM
    }
    return p;
}

// Función para alojar una página en RAM
void alojar_en_ram(Procesos *p, int page_index, Memoria *ram, int page_size) {
    if (ram->num_pages < ram->capacity) {
        ram->pages[ram->num_pages].process_id = p->id;
        ram->pages[ram->num_pages].page_id = page_index;
        ram->pages[ram->num_pages].in_ram = 1; // Página está en RAM
        ram->num_pages++;
        ram->memgastada += page_size;
        p->pages[page_index].in_ram = 1; // Actualizar estado de la página en el proceso
    }
}

// Función para alojar una página en Swap
void alojar_en_swap(Procesos *p, int page_index, Memoria *swap, int page_size) {
    if (swap->num_pages < swap->capacity) {
        swap->pages[swap->num_pages].process_id = p->id;
        swap->pages[swap->num_pages].page_id = page_index;
        swap->pages[swap->num_pages].in_ram = 0; // Página está en Swap
        swap->num_pages++;
        swap->memgastada += page_size;
    }
}

// Función principal para alojar páginas en RAM o Swap
void alojar_en_memoria(Procesos *p, Memoria *ram, Memoria *swap, int page_size) {
    pthread_mutex_lock(&memory_mutex); // Bloquear el mutex

    int paginas_necesarias = (p->size + page_size - 1) / page_size; // Redondeo hacia arriba
    int paginas_asignadas = 0;

    for (int i = 0; i < paginas_necesarias; i++) {
        if (ram->num_pages < ram->capacity) {
            alojar_en_ram(p, i, ram, page_size); // Intentar alojar en RAM
        } else if (swap->num_pages < swap->capacity) {
            alojar_en_swap(p, i, swap, page_size); // Intentar alojar en Swap
        } else {
            fprintf(stderr, "Error: No hay espacio suficiente en RAM ni Swap.\n");
            pthread_mutex_unlock(&memory_mutex); // Liberar el mutex antes de salir
            exit(1);
        }
        paginas_asignadas++;
    }

    // printf("Proceso %d: %d páginas asignadas.\n", p->id, paginas_asignadas);
    // printf("RAM: %d/%d MB usados.\n", ram->memgastada, ram->capacity * page_size);
    // printf("Swap: %d/%d MB usados.\n", swap->memgastada, swap->capacity * page_size);

    pthread_mutex_unlock(&memory_mutex); // Liberar el mutex
}


// Función para manejar un proceso en un hilo
void *proceso_hilo(void *arg) {
    int id = *(int *)arg;
    free(arg);

    // Tamaño aleatorio del proceso
    int process_size = (rand() % 128 + 1) * 4; // Multiplo de 4 MB
    Procesos *p = create_process(id, process_size, 4);

    extern Memoria RAM, SWAP;
    Alojar_en_memoria(p, &RAM, &SWAP, 4);

    free(p->pages);
    free(p);
    pthread_exit(NULL);
}

// Memorias globales
Memoria RAM, SWAP;

int main() {
    srand(time(NULL));

    // Tamaño de RAM y swap
    int ram_size = 512;  // MB
    int swap_size = 1024; // MB

    // Inicializar memoria
    inicializar_memoria(&RAM, 0, ram_size / 4);
    inicializar_memoria(&SWAP, 1, swap_size / 4);

    // Inicializar mutex
    pthread_mutex_init(&memory_mutex, NULL);

    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < MAX_PROCESSES; i++) {
        int *id = malloc(sizeof(int));
        *id = i;

        pthread_create(&threads[i % MAX_THREADS], NULL, proceso_hilo, id);

        if ((i + 1) % MAX_THREADS == 0) {
            for (int j = 0; j < MAX_THREADS; j++) {
                pthread_join(threads[j], NULL);
            }
        }

        sleep(2); // Simulación de tiempo entre procesos
    }

    // Liberar memoria
    free(RAM.pages);
    free(SWAP.pages);
    pthread_mutex_destroy(&memory_mutex);

    return 0;
}
