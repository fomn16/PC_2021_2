#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_quer_escrever = PTHREAD_MUTEX_INITIALIZER;

int num_leitores = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
	int *id;
        /* criando leitores */
	for (i = 0; i < NL ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, reader, (void *) (id));
	}
	 /* criando escritores */
	for (i = 0; i< NE; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&w[i], NULL, writer, (void *) (id));
	}
	pthread_join(r[0],NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {               					/* repere para sempre */
		pthread_mutex_lock(&lock_quer_escrever);	// verifica se algum escritor quer fazer a escrita. A execução é

		pthread_mutex_unlock(&lock_quer_escrever);   // congelada aqui se for o caso. lock_quer_escrever é obtido pela
											// menor quantidade de tempo possível, funcionando apenas como uma
											// verificação para os leitores, impedindo starvation dos escritores

		pthread_mutex_lock(&lock_nl);				// lock para uso da variável num_leitores é obtido

			num_leitores++;					// num_leitores é incrementado

			if(num_leitores == 1){				// num_leitores é testado. É importante lembrar que essa verificação 
											// também faz parte da região crítica (evitando condições de corrida neste if)
				
				pthread_mutex_lock(&lock_bd);		// lock_bd é obtido se este é o primeiro leitor a entrar no "pool"
			}
		pthread_mutex_unlock(&lock_nl);			// lock para uso da variável num_leitores é cedido

					read_data_base(i);       	/* acesso aos dados */

		pthread_mutex_lock(&lock_nl);				// lock para uso da variável num_leitores é obtido
			num_leitores--;					// num_leitores é decrementado
			if(num_leitores == 0){ 
				pthread_mutex_unlock(&lock_bd);	// lock_bd é cedido se este é o ultimo leitor a sair do "pool"
			}
		pthread_mutex_unlock(&lock_nl);			// lock para uso da variável num_leitores é cedido
		use_data_read(i);        				/* região não crítica */
	}
	pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {               					/* repete para sempre */
		think_up_data(i);        				/* região não crítica */

		pthread_mutex_lock(&lock_quer_escrever);	// lock_quer_escrever é obtido, fazendo com que leitores parem
											// de ser adicionados ao "pool" de leitores

			pthread_mutex_lock(&lock_bd);			// espera todos os leitores acabarem de ler e obtém lock_bd

		pthread_mutex_unlock(&lock_quer_escrever);	// lock_quer_escrever é liberado logo que lock_bd é obtido,
											// permitindo que leitores se "enfilerem" para ler enquanto a
											// escrita é feita, evitando starvation dos leitores.

	     		write_data_base(i);      		/* atualiza os dados */
		
			pthread_mutex_unlock(&lock_bd);		// lock_bd é liberado
	}
	pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep( rand() % 5 + 15);
}
