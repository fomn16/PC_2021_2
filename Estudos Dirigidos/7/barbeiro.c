/*
 * Problema do barbeiro dorminhoco.
 */ 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 50
#define N_CADEIRAS 5

sem_t sem_cadeiras, cadeiraBarbeiro, barbeiro, corte;

void * f_barbeiro(void *v) {

  while(1) {
    //...Esperar/dormindo algum cliente sentar na cadeira do barbeiro (e acordar o barbeiro)
    sem_wait(&barbeiro);
    sleep(1); //Cortar o cabelo do cliente 
    printf("Barbeiro cortou o cabelo de um cliente\n"); 
    //...Liberar/desbloquear o cliente
    sem_post(&corte);
  }
  pthread_exit(0);
}

void* f_cliente(void* v) {
  int id = *(int*) v;
  sleep(id%3);
  if(sem_trywait(&sem_cadeiras) == 0){ //conseguiu pegar uma cadeira de espera
    printf("Cliente %d entrou na barbearia \n",id);
    //... pegar/sentar a cadeira do barbeiro
    sem_wait(&cadeiraBarbeiro);
    //... liberar a sua cadeira de espera
    sem_post(&sem_cadeiras);
    //... acordar o barbeiro para cortar seu cabelo
    sem_post(&barbeiro);
    //... aguardar o corte do seu cabelo
    sem_wait(&corte);
    //... liberar a cadeira do barbeiro
    sem_post(&cadeiraBarbeiro);
    printf("Cliente %d cortou o cabelo e foi embora \n",id);

  }else{//barbearia cheia
	printf("Barbearia cheia, cliente %d indo embora\n",id);
  }

  pthread_exit(0);
}

int main() {
  pthread_t thr_clientes[N_CLIENTES], thr_barbeiro;
  int i, id[N_CLIENTES];

  sem_init(&sem_cadeiras, 0, N_CADEIRAS);
  sem_init(&cadeiraBarbeiro, 0, 1);
  sem_init(&barbeiro, 0, 0);
  sem_init(&corte, 0, 0);
  
  for (i = 0; i < N_CLIENTES; i++) {
    id[i] = i;
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id[i]);
  }

  pthread_create(&thr_barbeiro, NULL, f_barbeiro, NULL);
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Barbeiro assassinado */
  
  return 0;
}

