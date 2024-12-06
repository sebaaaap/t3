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

// Continuación de la función para alojar las páginas de un proceso en RAM o Swap
void Alojar_en_memoria(Procesos *p, Memoria *ram, Memoria *swap, int sizepagina) {
    int paginas_necesarias = p->size / sizepagina; // Número de páginas necesarias
    int paginas_asignadas = 0;

    // Intentar asignar páginas a la RAM
    for (int i = 0; i < paginas_necesarias; i++) {
        if (ram->num_pages < ram->capacity) {
            // Asignar página a RAM
            ram->pages[ram->num_pages] = p->pages[i]; // Copiar la página
            ram->pages[ram->num_pages].in_ram = 1; // En RAM
            ram->num_pages++; // Incrementar el número de páginas en RAM
            paginas_asignadas++;
        } else if (swap->num_pages < swap->capacity) {
            // No hay espacio en RAM, intentar asignar a Swap
            swap->pages[swap->num_pages] = p->pages[i]; // Copiar la página
            swap->pages[swap->num_pages].in_ram = 0; // En Swap
            swap->num_pages++; // Incrementar el número de páginas en Swap
            paginas_asignadas++;
        } else {
            // No hay espacio suficiente ni en RAM ni en Swap
            fprintf(stderr, "Error: No hay espacio suficiente en RAM ni en Swap. Terminando simulación.\n");
            exit(1);
        }
    }
}

// Función para finalizar un proceso
void finalizarproceso(So *sys, int id) {
    for (int i = 0; i < sys->num_processes; i++) {
        if (sys->processes[i]->id == id) {
            // Liberar la memoria de las páginas del proceso
            free(sys->processes[i]->pages);
            free(sys->processes[i]);
            // Mover el último proceso al lugar del que se elimina
            sys->processes[i] = sys->processes[sys->num_processes - 1];
            sys->num_processes--;
            printf("Proceso %d finalizado.\n", id);
            return;
        }
    }
    printf("Proceso %d no encontrado.\n", id);
}

// Función para simular el acceso a una dirección virtual
void simulaDvirtual(So *sys, Memoria *ram, Memoria *swap, int page_size) {
    int random_process_index = rand() % sys->num_processes;
    Procesos *p = sys->processes[random_process_index];
    int random_page_index = rand() % (p->size / page_size);
    
    // Verificar si la página está en RAM
    if (p->pages[random_page_index].in_ram) {
        printf("Acceso a la página %d del proceso %d: OK (en RAM)\n", random_page_index, p->id);
    } else {
        printf("Acceso a la página %d del proceso %d: Page Fault (en Swap)\n", random_page_index, p->id);
        
        // Simular el proceso de swap (reemplazo de página)
        // Aquí puedes implementar una política de reemplazo, por ejemplo, FIFO o LRU.
        // Para simplificar, vamos a eliminar la primera página en RAM.
        if (ram->num_pages > 0) {
            // Eliminar la primera página en RAM
            ram->num_pages--;
            printf("Página eliminada de RAM para hacer espacio.\n");
        } else {
            fprintf(stderr, "Error: No hay páginas en RAM para eliminar.\n");
            exit(1);
        }
        
        // Ahora, mover la página del swap a RAM
        ram->pages[ram->num_pages] = swap->pages[swap->num_pages - 1]; // Mover la última página del swap a RAM
        ram->pages[ram->num_pages].in_ram = 1; // Ahora está en RAM
        ram->num_pages++;
        swap->num_pages--; // Reducir el número de páginas en swap
    }
}

// Función principal
int main() {
    srand(time(NULL));

    // Parámetros de entrada
    int ram_memory_size = 512;  // Memoria Ram en MB
    int page_size = 4;  // Tamaño de cada página en MB
    int virtual_memory_size = rand() % (ram_memory_size * 4) + (ram_memory_size * 1.5); 

    // Inicializar el sistema y la memoria
    So sys;
    Memoria Ram, Virtual;
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
        sys.processes[sys.num_processes++] = p; // Agregar el proceso al sistema

        // Alojar el proceso en memoria
        Alojar_en_memoria(p, &Ram, &Virtual, page_size);

        sleep(2); // Esperar 2 segundos antes de crear el siguiente proceso
    }

    // Simulación de finalizar procesos y acceso a direcciones virtuales
    time_t start_time = time(NULL);
    while (difftime(time(NULL), start_time) < 30) { // Ejecutar durante 30 segundos
        sleep(5); // Esperar 5 segundos

        // Finalizar un proceso aleatorio
        if (sys.num_processes > 0) {
            int random_process_index = rand() % sys.num_processes;
            finalizarproceso(&sys, sys.processes[random_process_index]->id);
        }

        // Simular acceso a una dirección virtual
        if (sys.num_processes > 0) {
            simulaDvirtual(&sys, &Ram, &Virtual, page_size);
        }
    }

    // Liberar la memoria de RAM y Virtual
    free(Ram.pages);
    free(Virtual.pages);

    // Liberar procesos restantes
    for (int i = 0; i < sys.num_processes; i++) {
        free(sys.processes[i]->pages);
        free(sys.processes[i]);
    }

    return 0;
}