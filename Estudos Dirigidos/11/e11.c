#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 100  /* maximum matrix size */

pthread_barrier_t barrier;  
int size;

int matrix[MAXSIZE][MAXSIZE];
int partialR[MAXSIZE];
int R = 0;

void *Worker(void *);

int main(int argc, char *argv[]) {
	int i, j;

	size = atoi(argv[1]);

	if(size > MAXSIZE){
		printf("Tamanho muito grande!\n");
		return 0;	
	}

	pthread_t workerid[size];

	pthread_barrier_init(&barrier, NULL, size);

	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			matrix[i][j] = 1;

	for (i = 0; i < size; i++)
			partialR[i] = 0;

	int * id;
	for (i = 0; i < size; i++){
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&workerid[i], NULL, Worker, (void *) (id));
	}

	for (i = 0; i < size; i++){
		if(pthread_join(workerid[i], NULL)){
			printf("\n ERROR joining thread");
			exit(1);
		}
	}
	printf("Bye!\n");
}


void *Worker(void *arg) {
	int myid = *(int *)(arg);
	int i;

	int self = pthread_self();

	printf("worker %d (pthread id %d) has started\n", myid, self);
	for(i = 0; i < size; i++){
		partialR[myid] += matrix[myid][i];
	}

	pthread_barrier_wait(&barrier);

	if (myid == 0) {
		for(i = 0; i < size; i++){
			R += partialR[i];
		}
		printf("\n%d\n", R);
	}
}
