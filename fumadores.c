#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TABACO 		1
#define PAPEL 		2
#define FOSFOROS	3

sem_t papel_fosforos;   
sem_t tabaco_fosforos;
sem_t tabaco_papel;
sem_t interesados;    // 1: ningun fumador estA fumando.  0: un fumador esta fumando.

// ingredientes sobre la mesa
int ingrediente1 = 0;
int ingrediente2 = 0;

int numIteraciones = 0;
int ventas = 0;

char *getNombre(int x){
    char *nombre = malloc(sizeof(char) * 10);
    if (x == 1)
        strcpy(nombre, "tabaco");
 	if (x == 2)
 		strcpy(nombre, "papel");
 	if (x == 3)
 		strcpy(nombre, "fosforos");
    return nombre;
}

int sobreMesa(int x, int y){
	if ((ingrediente1 == x && ingrediente2 == y) || (ingrediente2 == x && ingrediente1 == y))
		return 1;
	return 0;
}

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

void *funcion_fumador(void *arg) {
    int id = (int) arg;
    int ingrediente = 0;    // ingrediente propio

    if (id == 1)
        ingrediente = TABACO;
    else if (id == 2)
        ingrediente = PAPEL;
    else if (id == 3)
        ingrediente = FOSFOROS;

    while (1){                          // como son compulsivos, siempre estAn a la espera de ingredientes
        if (ingrediente == TABACO){
            wait(&papel_fosforos);
        }
        if (ingrediente == FOSFOROS){
            wait(&tabaco_papel);
        }
        if (ingrediente == PAPEL){
            wait(&tabaco_fosforos);
        }

        printf("- Fumador %d fumando.....\n\n", id);
        sleep(3);
        signal(&interesados);
    }
}

void *funcion_vendedor(void *arg) {
    while (ventas < numIteraciones){
      	ingrediente1 = rand()%3 + 1;
      	do
      	{
      		ingrediente2 = rand()%3 + 1;
      	} while (ingrediente1 == ingrediente2);

        wait(&interesados);      	// espera hasta que tiene 3 potenciales interesados
        printf("[iteracion %d] Ingredientes en la mesa: %s y %s\n", ventas, getNombre(ingrediente1), getNombre(ingrediente2));
        sleep(1);
        if (sobreMesa(TABACO, FOSFOROS)){
            signal(&tabaco_fosforos);
        }
        if (sobreMesa(TABACO, PAPEL)){
            signal(&tabaco_papel);
        }
        if (sobreMesa(PAPEL, FOSFOROS)){
            signal(&papel_fosforos);
        }
        ventas+=1;
    }
}		

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Uso correcto: %s num-iteraciones\n", argv[0]);
      	exit(EXIT_FAILURE);
    }

    numIteraciones = atoi(argv[1]);
    srand(time(NULL));

    // inicializar semaforos
    init(&papel_fosforos, 0);
    init(&tabaco_fosforos, 0);
    init(&tabaco_papel, 0);
    init(&interesados, 1);

    printf("[INICIO] fumador %d posee %s\n", 1, getNombre(1));
    printf("[INICIO] fumador %d posee %s\n", 2, getNombre(2));
    printf("[INICIO] fumador %d posee %s\n", 3, getNombre(3));

    pthread_t vendedor, fumador1, fumador2, fumador3;
    
    pthread_create(&vendedor, NULL, funcion_vendedor, NULL);

    // se crean los fumadores
    pthread_create(&fumador1, NULL, funcion_fumador, (void *) 1);
    pthread_create(&fumador2, NULL, funcion_fumador, (void *) 2);
    pthread_create(&fumador3, NULL, funcion_fumador, (void *) 3);
    
    pthread_join(vendedor, NULL);  // se espera hasta que el vendedor acabe las ventas
    printf("fin\n");
}
