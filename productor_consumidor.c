#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TAM_BUFFER 5
#define PRODUCTORES 2
#define CONSUMIDORES 2
#define ELEMENTOS_POR_PRODUCTOR 10
#define TOTAL_ELEMENTOS (PRODUCTORES * ELEMENTOS_POR_PRODUCTOR)
#define FIN -1

typedef struct {
    int datos[TAM_BUFFER];
    int entrada;
    int salida;
} Buffer;

Buffer buffer = {{0}, 0, 0};
sem_t espacios_libres;
sem_t elementos_disponibles;
pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;

void insertar(int valor) {
    buffer.datos[buffer.entrada] = valor;
    buffer.entrada = (buffer.entrada + 1) % TAM_BUFFER;
}

int extraer(void) {
    int valor = buffer.datos[buffer.salida];
    buffer.salida = (buffer.salida + 1) % TAM_BUFFER;
    return valor;
}

void *productor(void *arg) {
    int id = *(int *)arg;
    for (int i = 1; i <= ELEMENTOS_POR_PRODUCTOR; i++) {
        int valor = id * 100 + i;
        sem_wait(&espacios_libres);
        pthread_mutex_lock(&mutex_buffer);
        insertar(valor);
        printf("Productor %d produjo el elemento %d\n", id, valor);
        pthread_mutex_unlock(&mutex_buffer);
        sem_post(&elementos_disponibles);
    }
    printf("Productor %d termino.\n", id);
    return NULL;
}

void *consumidor(void *arg) {
    int id = *(int *)arg;
    while (1) {
        sem_wait(&elementos_disponibles);
        pthread_mutex_lock(&mutex_buffer);
        int valor = extraer();
        pthread_mutex_unlock(&mutex_buffer);
        sem_post(&espacios_libres);

        if (valor == FIN) {
            printf("Consumidor %d recibio la senal de finalizacion.\n", id);
            break;
        }
        printf("Consumidor %d consumio el elemento %d\n", id, valor);
    }
    return NULL;
}

int main(void) {
    pthread_t h_productores[PRODUCTORES];
    pthread_t h_consumidores[CONSUMIDORES];
    int id_productores[PRODUCTORES] = {1, 2};
    int id_consumidores[CONSUMIDORES] = {1, 2};

    sem_init(&espacios_libres, 0, TAM_BUFFER);
    sem_init(&elementos_disponibles, 0, 0);

    printf("=== PROBLEMA PRODUCTOR-CONSUMIDOR ===\n");
    printf("Productores: %d | Consumidores: %d | Tamano del buffer: %d\n\n",
           PRODUCTORES, CONSUMIDORES, TAM_BUFFER);

    for (int i = 0; i < CONSUMIDORES; i++)
        pthread_create(&h_consumidores[i], NULL, consumidor, &id_consumidores[i]);
    for (int i = 0; i < PRODUCTORES; i++)
        pthread_create(&h_productores[i], NULL, productor, &id_productores[i]);

    for (int i = 0; i < PRODUCTORES; i++)
        pthread_join(h_productores[i], NULL);

    /* Un elemento especial permite que cada consumidor termine. */
    for (int i = 0; i < CONSUMIDORES; i++) {
        sem_wait(&espacios_libres);
        pthread_mutex_lock(&mutex_buffer);
        insertar(FIN);
        pthread_mutex_unlock(&mutex_buffer);
        sem_post(&elementos_disponibles);
    }

    for (int i = 0; i < CONSUMIDORES; i++)
        pthread_join(h_consumidores[i], NULL);

    printf("\nProceso terminado: se produjeron y consumieron %d elementos.\n", TOTAL_ELEMENTOS);
    sem_destroy(&espacios_libres);
    sem_destroy(&elementos_disponibles);
    pthread_mutex_destroy(&mutex_buffer);
    return 0;
}
