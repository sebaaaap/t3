#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCESSES 100 // num de procesos 
     

typedef struct {
    int id;     // identificador
    int size;  // Tamaño del proceso
    int *pages; // Arreglo de páginas del proceso
} Procesos;

typedef struct {
    int process_id;   // ID del proceso
    int size;  // tamaño de la pagina
    int in_ram;       // 1 si está en RAM, 0 si está en swap
} Pagina;


typedef struct {
    int id;           // Identificador de la memoria (0 para RAM, 1 para HDD)
    int *pages;       // Arreglo de páginas en memoria
    int num_pages;    // Número de páginas actuales
    int capacity;     // Capacidad total de la memoria (en número de páginas)
} Memoria;

typedef struct {
    Procesos *processes[MAX_PROCESSES]; //  array de procesos(punteros)
    int num_processes;  // num de procesos en el soazq
} So;

void Iniciarsistema(So *sys) {
    sys->num_processes = 0;  // Inicializa la cantidad de procesos activos
}

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
    p->pages = malloc(num_pages * sizeof(int));

    if (p->pages == NULL) {
        fprintf(stderr, "Error al asignar memoria para las páginas\n");
        free(p);
        exit(1);
    }

    // Inicializar páginas con valores aleatorios
    for (int i = 0; i < num_pages; i++) {
        p->pages[i] = rand() % 1000;  // Ejemplo de valor aleatorio
    }

    return p;
}


void Alojar_en_memoria(So *sys, Procesos *p, Memoria *ram, Memoria *Hdd) {
    // Aquí iría la lógica para asignar las páginas a la RAM o swap
}

void Page_faoult_y_remplazo(Procesos *p, Memoria *ram, Memoria *Hdd) {
    // Aquí gestionas los page faults y políticas de reemplazo
}

void Simular_acceso(So *sys, Memoria *ram, Memoria *Hdd) {
    // Generar acceso aleatorio a una dirección virtual
}

void Limpiarmemoria(So *sys) {
    // Limpiar memoria
}


void inicializar_memoria(Memoria *mem, int id, int capacity) {
    mem->id = id;
    mem->capacity = capacity;
    mem->num_pages = 0;
    mem->pages = malloc(capacity * sizeof(int));
    for (int i = 0; i < capacity; i++) {
        mem->pages[i] = -1;  // Indica que la página está vacía
    }
}

int main() {
    srand(time(NULL));
    
    // Parámetros de entrada
    int ram_memory_size = 512;  // Memoria Ram en MB
    int page_size = 4;  // Tamaño de cada página en MB
    int virtual_memory_size = rand() % (ram_memory_size * 4) + (ram_memory_size * 1.5);  // Memoria virtual aleatoria

    So sys;
    
    Memoria Ram, Virtual;



    Iniciarsistema(&sys);

    inicializar_memoria(&Ram, 0, ram_memory_size);
    inicializar_memoria(&Virtual, 1, virtual_memory_size);


     for (int i = 0; i < 100; i++) {
        int process_size;

        // Generar un tamaño de proceso que sea par y múltiplo del tamaño de la página
        do {
            process_size = (rand() % (ram_memory_size / page_size)) * page_size; // Tamaño en múltiplos de page_size
        } while (process_size % 2 != 0 || process_size == 0); // Aseguramos que sea par y no cero

        // Crear el proceso
        Procesos *p = create_process(i, process_size, page_size);

        // Imprimir información del proceso
        printf("Proceso ID: %d, Tamaño: %d bytes, Número de páginas: %d\n", 
               p->id, p->size, p->size / page_size);
        
        // Liberar memoria de las páginas
        free(p->pages);
        free(p);
    }

    // Simulación de accesos a páginas y reemplazo de páginas
    sleep(30);
    
    // Simular_acceso(&sys, &ram, &Hdd);

    // cleanup(&sys);
    return 0;
}
