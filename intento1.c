#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCESSES 100 // num de procesos 
#define MAX_MEMORY 1024 // Tamaño máximo de memoria (en MB)
#define PAGE_SIZE 4     // Tamaño de cada página (en MB)

typedef struct {
    int id;     // identificador
    int size;  // Tamaño del proceso
    int *pages; // Arreglo de páginas del proceso
    int num_pages; //limite de paginas 
} Procesos;


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

Procesos *create_process(int id, int size, int num_pages) {
    Procesos *p = malloc(sizeof(Procesos));
    p->id = id;
    p->size = size;
    p->num_pages = num_pages;
    p->pages = malloc(num_pages * sizeof(int));
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
    int physical_memory_size = 512;  // Memoria física en MB
    int page_size = 4;  // Tamaño de cada página en MB
    int virtual_memory_size = rand() % (physical_memory_size * 4) + (physical_memory_size * 1.5);  // Memoria virtual aleatoria

    So sys;
    
    Memoria ram, Hdd;



    Iniciarsistema(&sys);

    inicializar_memoria(&ram, 0, physical_memory_size / PAGE_SIZE);
    inicializar_memoria(&Hdd, 1, virtual_memory_size / PAGE_SIZE);

    // Simulación de procesos
    for (int i = 0; i < 10; i++) {

        int process_size = rand() % 100 + 10;  // Tamaño del proceso

        int num_pages = (process_size + page_size - 1) / page_size;

        Procesos *p = create_process(i, process_size, num_pages);

        sys.processes[sys.num_processes++] = p;

        Memoria ram = { .id = 0, .pages = malloc(physical_memory_size * sizeof(int)), .num_pages = physical_memory_size / page_size };
        Memoria Hdd = { .id = 1, .pages = malloc(virtual_memory_size * sizeof(int)), .num_pages = virtual_memory_size / page_size };

        allocate_memory(&sys, p, &ram, &Hdd);
    }

    // Simulación de accesos a páginas y reemplazo de páginas
    sleep(30);
    
    // Simular_acceso(&sys, &ram, &Hdd);

    // cleanup(&sys);
    return 0;
}
