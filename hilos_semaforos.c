#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define LIMITE 10000
#define INICIO_CRITICO 4000
#define FIN_CRITICO 6000

sem_t sem_critica;

void *contar(void *arg) {
    long id = (long)arg;

    for (int i = 1; i <= LIMITE; i++) {
        /* El hilo solicita acceso exclusivo justo antes de imprimir 4000. */
        if (i == INICIO_CRITICO) {
            sem_wait(&sem_critica);
            printf("Hilo %ld: entra a la seccion critica (4000-6000)\n", id);
        }

        printf("Hilo %ld: %d\n", id, i);

        /* Libera el semaforo despues de imprimir 6000. */
        if (i == FIN_CRITICO) {
            printf("Hilo %ld: sale de la seccion critica\n", id);
            sem_post(&sem_critica);
        }
    }

    return NULL;
}

int main(void) {
    pthread_t hilo1, hilo2;

    if (sem_init(&sem_critica, 0, 1) != 0) {
        perror("Error al inicializar el semaforo");
        return EXIT_FAILURE;
    }

    if (pthread_create(&hilo1, NULL, contar, (void *)1L) != 0) {
        perror("Error al crear el hilo 1");
        sem_destroy(&sem_critica);
        return EXIT_FAILURE;
    }

    if (pthread_create(&hilo2, NULL, contar, (void *)2L) != 0) {
        perror("Error al crear el hilo 2");
        pthread_join(hilo1, NULL);
        sem_destroy(&sem_critica);
        return EXIT_FAILURE;
    }

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    sem_destroy(&sem_critica);
    printf("\nAmbos hilos terminaron correctamente.\n");
    return EXIT_SUCCESS;
}
