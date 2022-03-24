#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 3 //número de produtores
#define CN 2 // número de consumidores
#define N 5  //tamanho do buffer

void * produtor(void * meuid);
void * consumidor (void * meuid);

//variavel condicional dos produtores
pthread_cond_t c_produtor = PTHREAD_COND_INITIALIZER;
//variavel condicional dos consumidores
pthread_cond_t c_consumidor = PTHREAD_COND_INITIALIZER;
//lock para uso das variaveis da fila
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//variaveis da fila, entende-se que a fila como 
//um buffer circular. m e n sao indices para
//o mesmo
int m, n; // m = inicio da fila, n = fim da fila;

//nada foi mudado na main alem da linha comentada
int main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i;                      
  int *id;

  //inicializando fila vazia no indice 0
  m = n = 0;

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
    sleep(5);                                 //delay para facilitar visualizacao
    pthread_mutex_lock(&lock);                //entra na regiao critica
    if(m - n != 1 && m - n != -(N - 1)){      //se a fila nao esta cheia, adiciona item
      printf("produtor %d produzindo item %d\n", *((int*)pi), n);
      n = (n + 1)%N;                          //atualiza fim da fila
      pthread_cond_signal(&c_consumidor);     //ativa 1 consumidor
    }
    else{
      pthread_cond_wait(&c_produtor, &lock);  //se esta cheia, espera
    }
    pthread_mutex_unlock(&lock);              //sai da regiao critica
  }
  pthread_exit(0);
}

void * consumidor (void* pi)
{
 while(1)
  {
    sleep(5);                                 //delay para facilitar visualizacao
    pthread_mutex_lock(&lock);                //entra na regiao critica
    if(m != n){                               //se ha itens na fila
      printf("consumidor %d consumindo item %d\n", *((int*)pi), m);
      m = (m + 1)%N;                          //atualiza inicio da fila
      pthread_cond_signal(&c_produtor);       //ativa 1 produtor
    }
    else{
      pthread_cond_wait(&c_consumidor, &lock);//se fila esta vazia, espera
    }
    pthread_mutex_unlock(&lock);              //sai da regiao critica
  }
  pthread_exit(0);
}
