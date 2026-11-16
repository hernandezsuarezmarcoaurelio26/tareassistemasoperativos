#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_FILOSOFOS 5
#define VECES_COMER 3

pthread_mutex_t tenedores[NUM_FILOSOFOS];

void *filosofo(void *arg) {
    int id = *(int *)arg;
    int izquierdo = id;
    int derecho = (id + 1) % NUM_FILOSOFOS;

    for (int vuelta = 1; vuelta <= VECES_COMER; vuelta++) {
        printf("Filosofo %d esta pensando (ronda %d).\n", id + 1, vuelta);
        usleep(10000);

        /* Tomar primero el tenedor de menor indice evita el ciclo de espera. */
        int primero = (izquierdo < derecho) ? izquierdo : derecho;
        int segundo = (izquierdo < derecho) ? derecho : izquierdo;

        pthread_mutex_lock(&tenedores[primero]);
        printf("Filosofo %d tomo el tenedor %d.\n", id + 1, primero + 1);
        pthread_mutex_lock(&tenedores[segundo]);
        printf("Filosofo %d tomo el tenedor %d.\n", id + 1, segundo + 1);

        printf("Filosofo %d ESTA COMIENDO (ronda %d).\n", id + 1, vuelta);
        usleep(10000);

        pthread_mutex_unlock(&tenedores[segundo]);
        pthread_mutex_unlock(&tenedores[primero]);
        printf("Filosofo %d dejo los tenedores.\n", id + 1);
    }
    printf("Filosofo %d termino de comer.\n", id + 1);
    return NULL;
}

int main(void) {
    pthread_t hilos[NUM_FILOSOFOS];
    int ids[NUM_FILOSOFOS];

    printf("=== PROBLEMA DE LA CENA DE LOS FILOSOFOS ===\n");
    printf("Hay %d filosofos y %d tenedores. Cada filosofo comera %d veces.\n\n",
           NUM_FILOSOFOS, NUM_FILOSOFOS, VECES_COMER);

    for (int i = 0; i < NUM_FILOSOFOS; i++)
        pthread_mutex_init(&tenedores[i], NULL);

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, filosofo, &ids[i]);
    }
    for (int i = 0; i < NUM_FILOSOFOS; i++)
        pthread_join(hilos[i], NULL);

    printf("\nTodos los filosofos terminaron. No ocurrio interbloqueo.\n");
    for (int i = 0; i < NUM_FILOSOFOS; i++)
        pthread_mutex_destroy(&tenedores[i]);
    return 0;
}
