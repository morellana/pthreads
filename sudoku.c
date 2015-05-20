/*
Operating System Concepts
Chapter 4: Threads
Project 1: Sudoku Solution Validator

(1) A thread to check that each column contains the digits 1 through 9
(2) A thread to check that each row contains the digits 1 through 9
(3) Nine threads to check that each of the 3 × 3 subgrids contains the digits 1 through 9
*/



#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define FILAS 			9
#define COLUMNAS 		9

int sudoku[FILAS][COLUMNAS];		// sudoku sudoku ingresado por usuario
int error = 0; 						// al comienzo se asume que el puzle no tiene error

typedef struct
{
	int fila;
	int columna;
	int grilla;
} params;

/*	Recorre todas las columnas verificando
	que cada una tenga los numeros del 1 al 9 */
void *revisar_columnas (void *parametros)
{
	for (int i = 0; i < COLUMNAS; ++i)
	{
		for (int num = 1; num < 10; num++)
		{
			int encontrado = 0;
			int falta = -1;
			for (int j = 0; j < FILAS; ++j)
			{
				if (num == sudoku[j][i])
				{
					encontrado = 1;
				} else {
					falta = num;
				}
			}
			if (!encontrado)
			{
				error = 1;
				//printf("falta el numero %d en la columna %d\n", falta, i);
			}
		}
	}
}

/*	Recorre todas las filas verificando
	que cada una tenga los numeros del 1 al 9 */
void *revisar_filas (void *parametros)
{
	for (int i = 0; i < FILAS; ++i)
	{
		for (int num=1; num<10; num++)
		{
			int encontrado = 0;  				// no encontrado
			int falta = -1;
			for (int j = 0; j < COLUMNAS; j++)
			{
				if (num == sudoku[i][j])
				{
					encontrado = 1;
				}
				else {
					falta = num;
				}
			}
			if (!encontrado)
			{
				error = 1;
				//printf("falta el numero %d en la fila %d\n", falta, i);
				// break; 			//informar solo un error. al quitar informa por fila
			}
		}
	}
}

/* Revisa grilla de 3x3 verificando que
   tenga los numeros del 1 al 9 */
void *revisar_sub_grilla (void *parametros)
{
	params *p = (params *) parametros;
	int fila = p->fila;
	int columna = p->columna;
	for (int num = 1; num < 10; ++num)
	{
		int encontrado = 0;       // no encontrado
		int falta = -1;
		for (int i = 0; i<3; ++i)
		{
			for (int j = 0; j<3; ++j)
			{
				if (sudoku[fila+i][columna+j] == num) // encontrado
				{
					encontrado = 1;
				} else {
					falta = num;
				}
			}
		}
		if (!encontrado)
		{
			error = 1;
			//printf("Invalido. falta %d en grilla %d\n", falta, p->grilla);
		}
	}
}

int main(int argc, char const *argv[])
{
	// leer sudoku
	for (int i = 0; i < FILAS; ++i)
	{
		for (int j = 0; j < COLUMNAS; ++j)
		{
			scanf ("%d", &sudoku[i][j]);
		}
	}	
	
	// identificadores
	pthread_t thread_rows_check;
	pthread_t thread_cols_check;
	pthread_t threads_grill_check[9];

	// crear threads
	pthread_create (&thread_rows_check, NULL, revisar_filas, NULL);
	pthread_create (&thread_cols_check, NULL, revisar_columnas, NULL);

	int pos = 0;
	for (int i = 0; i < FILAS; i+=3)
	{
		for (int j = 0; j < COLUMNAS; j+=3)
		{
			params *data = (params *) malloc(sizeof(params));
			data->fila = i;
			data->columna = j;
			data->grilla = pos;
			
			pthread_create(&threads_grill_check[pos], NULL, revisar_sub_grilla, (void *) data);
			pthread_join (threads_grill_check[pos], NULL);
			pos++;
		}
	}

	// esperar a que los threads hijos terminen
	pthread_join (thread_rows_check, NULL);
	pthread_join (thread_cols_check, NULL);
 
 	// resultado
 	if (!error)
 		printf("Correcto\n");
 	else
 		printf("Incorrecto\n");
	
	return 0;
}