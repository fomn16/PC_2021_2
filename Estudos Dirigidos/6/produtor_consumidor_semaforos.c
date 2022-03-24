#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 5 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

void * produtor(void * meuid);
void * consumidor (void * meuid);

sem_t empty, full;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void main(argc, argv)
int argc;
char *argv[];
{
  sem_init(&empty, 0, N);
  sem_init(&full, 0, 0);

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi)
{
 
  while(1)
  {
      sleep(rand() % 10);
      sem_wait(&empty);
      pthread_mutex_lock(&lock);   
      printf("Produtor %d produziu\n", *((int*)pi));
      pthread_mutex_unlock(&lock); 
      sem_post(&full);
  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
 while(1)
  {
      sleep(rand() % 10); 
      sem_wait(&full);
      pthread_mutex_lock(&lock);  
      printf("Consumidor %d consumiu\n", *((int*)pi));
      pthread_mutex_unlock(&lock);
      sem_post(&empty);
  }
  pthread_exit(0);
}
