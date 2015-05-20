/*
Ejercicio de sincronización

Se ha inaugurado un nuevo circuito turístico en el Cerro Condell, que contempla una serie de senderos para
recorrer a pie, y una Victoria monoplaza para recorrer otros sectores. Los visitantes normalmente visitan los
senderos, caminando por un rato, y posteriormente les gusta continuar su recorrido en el vehículo tirado por
caballo.

Cuando la victoria está disponible, el visitante avisa al conductor que quiere viajar y sube. El conductor pasea
a su pasajero por un rato, notificándole cuando el paseo ha terminado. El visitante baja entonces y continúa su
recorrido a pie, dejando la victoria disponible para otros turistas.

Desarrolle una simulación de este escenario, en la que los visitantes y la victoria son modelados como procesos
o threads. Utilice semáforos para proveer la sincronización necesaria. Detalle claramente el propósito de cada
semáforo utilizado, y su valor inicial. En la medida de lo posible, no utilice otras variables que no sean
semáforos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX_RETARDO 5;

sem_t victoria;
sem_t pasajero;
sem_t mutex; // para controlar tiempo que el visitante esta sobre la victoria

int finalizado = 0;

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

void *funcion_victoria(void *arg){
	while(!finalizado){
		wait(&pasajero); // espera por pasajero
		//printf("Victoria a cogido un pasajero\n");

		sleep(rand()%5);   // Victoria avanza con pasajero
		signal(&mutex);
	}
	printf("--------------la victoria acaba su recorrido diario!\n");
} 

void *funcion_visitante(void *arg){
	int i2 = 0;
	while(i2 < 1){
		int i = (int) arg;
		printf("visitante %d caminando...\n", i);	

		sleep(rand()%10);

		wait(&victoria); 	// espera por Victoria monoplaza
		printf("visitante %d usando Victoria...\n", i);

		signal (&pasajero);
		wait(&mutex);  		// espera a que la Victoria finalice el recorrido
		signal(&victoria);  // deja Victoria disponible para otro visitantee

		i2 += 1;
	}
	//pthread_exit();
} 

int main(int argc, char const *argv[])
{
	if (argc != 2){
		printf("Uso: %s num-visitantes\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int num_visitantes = atoi(argv[1]);
	pthread_t vic, visitantes[num_visitantes];

	srand(time(NULL));
	init (&mutex, 0);
	init (&pasajero, 0); // victoria espera por pasajero
	init (&victoria, 1); // disponible al primer visitante que la solicite

	pthread_create (&vic, NULL, funcion_victoria, NULL);

	for (int i = 0; i < num_visitantes; ++i)
	{
		pthread_create(&visitantes[i], NULL, funcion_visitante, (void *) i);
		
	}

	for (int i = 0; i < num_visitantes; ++i)
	{
		pthread_join(visitantes[i], NULL);
	}
	
	finalizado = 1;
	signal(&pasajero);
	pthread_join(vic, NULL);
	

	//pthread_exit(NULL);


	return 0;
}