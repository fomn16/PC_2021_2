#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

void *canibal(void*meuid);
void *cozinheiro(int m);

int n_porcoes = 0;
int m;

//variavel condicional dos produtores
pthread_cond_t c_canibal = PTHREAD_COND_INITIALIZER;
//variavel condicional dos consumidores
pthread_cond_t c_cozinheiro = PTHREAD_COND_INITIALIZER;
//lock para uso das variaveis da fila
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) {
    //pegar uma porção de comida e acordar o cozinheiro se as porções acabaram
    pthread_mutex_lock(&lock);                  //entrando na regiao critica
      while(!n_porcoes)                         //enquanto nao houver porcoes
        pthread_cond_wait(&c_canibal, &lock);   //espera

      n_porcoes --;                             //ha porcoes, pega uma porcao
      if(!n_porcoes)                            //se essa foi a ultima porcao
        pthread_cond_signal(&c_cozinheiro);     //acorda cozinheiro
    pthread_mutex_unlock(&lock);                //sai da regiao critica

    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(10);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
    //dormir enquanto tiver comida
    pthread_mutex_lock(&lock);                  //entrando na regiao critica
      if(n_porcoes)                             //se ha porcoes
        pthread_cond_wait(&c_cozinheiro, &lock);//dorme
    pthread_mutex_unlock(&lock);                //saindo da regiao critica

    printf("cozinheiro: vou cozinhar\n");
    sleep(20);

    //acordar os canibais
    pthread_mutex_lock(&lock);                  //entrando na regiao critica
      n_porcoes += m;                           //incrementa porcoes
        pthread_cond_broadcast(&c_canibal);     //acorda canibais
    pthread_mutex_unlock(&lock);                //saindo da regiao critica
   }

}
