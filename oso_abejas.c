#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t lleno;   
sem_t vacio;
sem_t mutex;

int numAbejas    = 0;
int numPorciones = 0;
int porciones    = 0;   // variable para la cantidad de porciones en un instante de t

void init(sem_t *sem, int valor_inicial) {
	if (sem_init(sem, 0, valor_inicial)) {
    	printf("Error inicializando semaforo\n");
        exit(EXIT_FAILURE);
    }
}

void wait(sem_t *sem) {
   	if (sem_wait(sem)) {
   		printf("Error fatal haciendo sem_wait()\n");
      	exit(EXIT_FAILURE);
    }
}

void signal(sem_t *sem) {
   	if (sem_post(sem)) {
      	printf("Error fatal haciendo sem_post()\n");
      	exit(EXIT_FAILURE);
    }
}

void *funcion_abeja(void *arg) {
    int i = (int) arg;
    while(1){
        wait(&vacio);   // se hacen H peticiones al semaforo
        wait(&mutex);
        porciones++;
        printf("%d porciones en el recipiente [abeja %d]\n", porciones, i);
        if (porciones == numPorciones){
            signal(&lleno);
        }
        sleep(1);
        signal(&mutex);
    }
}

void *funcion_oso(void *arg) {
    while (1){
        wait(&lleno);  // oso espera hasta que el recipiente este lleno
        porciones = 0; // come
        printf("Oso esta comiendo.....\n");
        sleep(3);
        for (int i = 0; i < numPorciones; ++i)
        {
            signal(&vacio);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso correcto: %s num-abejas num-porciones\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numAbejas    = atoi(argv[1]);
    numPorciones = atoi(argv[2]);

    srand(time(NULL));

    init(&lleno, 0);
    init(&vacio, numPorciones);
    init(&mutex, 1);

    pthread_t oso, abejas[numAbejas];

    pthread_create (&oso, NULL, funcion_oso, NULL);

    for (int i = 0; i < numAbejas; ++i)
    {
        pthread_create(&abejas[i], NULL, funcion_abeja, (void *) i);
    }

    pthread_join(oso, NULL);
}