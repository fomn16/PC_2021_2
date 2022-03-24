#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

#define AB 0
#define BA 1
#define GAB 2
#define GBA 3

#define N_GRUPOS 4

//lock que restringe o acesso num_macacos
pthread_mutex_t lock_nm = PTHREAD_MUTEX_INITIALIZER;

//lock que restringe o acesso às variávis de controle de vez
pthread_mutex_t lock_controle_vez = PTHREAD_MUTEX_INITIALIZER;

int num_macacos = 0;

//variaveis de controle de vez
int quer[N_GRUPOS] = {0};                                   //guarda quantos macacos de cada grupo querem usar a corda
                                                            //ou se o grupo esta na fila
int fila [N_GRUPOS] = {AB, -10, -10, -10};                                 //guarda grupo que esta usando a corda (fila[vez_atual])
                                                            //assim como quais os proximos grupos que irao usa-la
int vez_atual = 0;                                          //posicao atual da fila
int vez_escrita = 0;                                        //posicao de inclusao na fila
    
void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
        sleep(rand() % 10);                                 //macacos chegam de tempos em tempos para atravessar

        //Procedimentos para acessar a corda
        pthread_mutex_lock(&lock_controle_vez);             //entrando na região crítica de acesso às variáveis de controle de vez
        if(fila[vez_atual] != AB && quer[AB] != -1){        //se o macaco não pertence ao grupo atual, e seu grupo não está na fila
            quer[AB]++;                                     //incrementa o numero de macacos deste grupo que quer usar a corda
            if(quer[AB] > 5){                               //se macacos o suficiente deste grupo querem usar a corda
                quer[AB] = -1;                              //indica que o grupo entrou na fila
                vez_escrita = (vez_escrita + 1) % N_GRUPOS; //atualiza a posição de escrita na fila
                fila[vez_escrita] = AB;                     //inclui este grupo na fila
                if(fila[vez_atual] >= 0)
                fila[vez_atual] = -fila[vez_atual] - 1;         //tira grupo atual da fila
            }
        }
        pthread_mutex_unlock(&lock_controle_vez);           //saindo da região crítica de acesso às variáveis de controle de vez
        printf("a1, %d, %d, %d, %d : %d %d\n", fila[0], fila[1], fila[2], fila[3], vez_atual, vez_escrita);
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        //esta parte do codigo faz com que macacos fiquem
        //presos neste loop ate que seja a vez de seu grupo
        //de usar a corda
        int c = 0;                                          //variável de controle temporária, usada para impedir deadlocks
        while(!c){
            pthread_mutex_lock(&lock_controle_vez);         //entrando na região crítica de acesso às variáveis de controle de vez
            c = fila[vez_atual] == AB;        
            pthread_mutex_unlock(&lock_controle_vez);       //saindo da região crítica de acesso às variáveis de controle de vez
        }
        
        pthread_mutex_lock(&lock_controle_vez);         //entra na região crítica de acesso às variáveis quer e vez.
        pthread_mutex_lock(&lock_nm);                       //lock é obtida para acesso a num_macacos 

        num_macacos++;                                      //número de macacos usando a corda é incrementado 

        if(num_macacos == 1){                               //se este é o primeiro macaco deste grupo a acessar a corda
            
                quer[AB] = 0;                               //sinaliza que não há macacos de seu grupo esperando para acessar a corda
            
        }
        
        pthread_mutex_unlock(&lock_nm);                     //lock para acesso a num_macacos é cedida 
        pthread_mutex_unlock(&lock_controle_vez);       //sai da região crítica de acesso às variáveis quer e vez.

        printf("Macaco %d passado de A para B \n",i);
        sleep(5);

        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_controle_vez);     //entra na região crítica de acesso às variáveis quer e vez. 
        pthread_mutex_lock(&lock_nm);                       //lock é obtida para acesso a num_macacos  
            num_macacos--;                                  //número de macacos usando a corda é decrementado 
            if(num_macacos == 0){                           //se este é o último macaco deste grupo a acessar a corda

                if(vez_escrita != vez_atual){               //verifica se algum outro grupo quer usar a corda
                    vez_atual = (vez_atual + 1) % N_GRUPOS; //atualiza a fila (da a vez ao proximo grupo)
                }
                
            }
        pthread_mutex_unlock(&lock_nm);                 //lock para acesso a num_macacos é cedida
        pthread_mutex_unlock(&lock_controle_vez);   //sai da região crítica de acesso às variáveis quer e vez.
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
        //sleep(rand() % 10);                             //macacos chegam de tempos em tempos para atravessar

        //Procedimentos para acessar a corda
        pthread_mutex_lock(&lock_controle_vez);             //entrando na região crítica de acesso às variáveis de controle de vez
        if(fila[vez_atual] != BA && quer[BA] != -1){        //se o macaco não pertence ao grupo atual, e seu grupo não está na fila
            quer[BA]++;                                     //incrementa o numero de macacos deste grupo que quer usar a corda
            if(quer[BA] > 5){                               //se macacos o suficiente deste grupo querem usar a corda
                quer[BA] = -1;                              //indica que o grupo entrou na fila
                vez_escrita = (vez_escrita + 1) % N_GRUPOS; //atualiza a posição de escrita na fila
                fila[vez_escrita] = BA;                     //inclui este grupo na fila
                if(fila[vez_atual] >= 0)
                fila[vez_atual] = -fila[vez_atual] - 1;         //tira grupo atual da fila
            }
        }
        pthread_mutex_unlock(&lock_controle_vez);           //saindo da região crítica de acesso às variáveis de controle de vez
printf("B1, %d, %d, %d, %d : %d %d\n", fila[0], fila[1], fila[2], fila[3], vez_atual, vez_escrita);
        
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        //esta parte do codigo faz com que macacos fiquem
        //presos neste loop ate que seja a vez de seu grupo
        //de usar a corda
        int c = 0;                                          //variável de controle temporária, usada para impedir deadlocks
        while(!c){
            pthread_mutex_lock(&lock_controle_vez);         //entrando na região crítica de acesso às variáveis de controle de vez
            c = fila[vez_atual] == BA;               
            pthread_mutex_unlock(&lock_controle_vez);       //saindo da região crítica de acesso às variáveis de controle de vez
        }

        pthread_mutex_lock(&lock_controle_vez);         //entra na região crítica de acesso às variáveis quer e vez.
        pthread_mutex_lock(&lock_nm);                       //lock é obtida para acesso a num_macacos
        num_macacos++;                                      //número de macacos usando a corda é incrementado 
        if(num_macacos == 1){                               //se este é o primeiro macaco deste grupo a acessar a corda
                quer[BA] = 0;                               //sinaliza que não há macacos de seu grupo esperando para acessar a corda
        }
        pthread_mutex_unlock(&lock_nm);                     //lock para acesso a num_macacos é cedida 
        pthread_mutex_unlock(&lock_controle_vez);       //sai da região crítica de acesso às variáveis quer e vez.

        printf("Macaco %d passado de B para A \n",i);
        sleep(5);

        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_controle_vez);     //entra na região crítica de acesso às variáveis quer e vez. 
        pthread_mutex_lock(&lock_nm);                       //lock é obtida para acesso a num_macacos  
            num_macacos--;                                  //número de macacos usando a corda é decrementado 
            if(num_macacos == 0){                           //se este é o último macaco deste grupo a acessar a corda
                if(vez_escrita != vez_atual){               //verifica se algum outro grupo quer usar a corda
                    vez_atual = (vez_atual + 1) % N_GRUPOS; //atualiza a fila (da a vez ao proximo grupo)
                } 
            }
        pthread_mutex_unlock(&lock_nm);                     //lock para acesso a num_macacos é cedida
        pthread_mutex_unlock(&lock_controle_vez);   //sai da região crítica de acesso às variáveis quer e vez.
    }
    pthread_exit(0);
}
/*
void * gorilaAB(void * a){
    while(1){
        sleep(rand() % 15);                             //gorilas chegam de tempos em tempos para atravessar
        //Procedimentos para acessar a corda

        //esta parte do código verifica se a lock_trocar deve ser obtida.
        //ela será obtida se a variável temporária c for setada (c = 1).
        //a lógica para setar ou não a variável é a seguinte:
        //Se o gorila pertence ao grupo que já está usando a corda, a variável
        //deve ser setada, pois ele deve ser impedido de acessar a corda caso
        //algum outro grupo queira usá-la.
        //Se não, c deve ser setada. A checagem quer[GAB] != -1 ocorre
        //para impedir que o mesmo grupo use o lock_trocar várias vezes
        //(apenas 1 gorila nessa direcao irá tentar obter o lock_trocar, o resto firacá preso no comando
        //pthread_mutex_lock(&lock_corda))
        pthread_mutex_lock(&lock_controle_vez);             //entrando na região crítica de acesso às variáveis quer e vez.
        if(vez == GAB)
            c = 1;
        else if(quer[GAB] != -1){
            c = 1;
            quer[GAB] = -1;
        }
        pthread_mutex_unlock(&lock_controle_vez);           //saindo da região crítica de acesso às variáveis quer e vez.
        if(c)
            pthread_mutex_lock(&lock_trocar);           //verifica se algum outro grupo quer usar a corda, ou 
                                                        //sinaliza a vontade de usa-la     

        pthread_mutex_lock(&lock_corda);                //obtem a corda           
        pthread_mutex_lock(&lock_controle_vez);             //entra na região crítica de acesso às variáveis quer e vez.
            vez = GAB;                                  //sinaliza que a vez é sua
            quer[GAB] = 0;                              //sinaliza que não há gorilas esperando para acessar a corda deste lado
        pthread_mutex_unlock(&lock_controle_vez);           //sai da região crítica de acesso às variáveis quer e vez.
        pthread_mutex_unlock(&lock_trocar);             //A corda já foi obtida, lock_trocar pode ser cedida

        printf("Gorila passado de A para B \n");
        
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        sleep(20);

        //Procedimentos para quando sair da corda
        
        pthread_mutex_unlock(&lock_corda);              //cede o uso da corda
    }
    pthread_exit(0);
}


void * gorilaBA(void * a){
    while(1){
        sleep(rand() % 15);                             //gorilas chegam de tempos em tempos para atravessar
        //Procedimentos para acessar a corda
        int c = 0;                                      //variável de controle temporária, usada para impedir deadlocks
        //esta parte do código verifica se a lock_trocar deve ser obtida.
        //ela será obtida se a variável temporária c for setada (c = 1).
        //a lógica para setar ou não a variável é a seguinte:
        //Se o gorila pertence ao grupo que já está usando a corda, a variável
        //deve ser setada, pois ele deve ser impedido de acessar a corda caso
        //algum outro grupo queira usá-la.
        //Se não, c deve ser setada. A checagem quer[GBA] != -1 ocorre
        //para impedir que o mesmo grupo use o lock_trocar várias vezes
        //(apenas 1 gorila nessa direcao irá tentar obter o lock_trocar, o resto firacá preso no comando
        //pthread_mutex_lock(&lock_corda))
        pthread_mutex_lock(&lock_controle_vez);             //entrando na região crítica de acesso às variáveis quer e vez.
        if(vez == GBA)
            c = 1;
        else if(quer[GBA] != -1){
            c = 1;
            quer[GBA] = -1;
        }
        pthread_mutex_unlock(&lock_controle_vez);           //saindo da região crítica de acesso às variáveis quer e vez.
        if(c)
            pthread_mutex_lock(&lock_trocar);           //verifica se algum outro grupo quer usar a corda, ou 
                                                        //sinaliza a vontade de usa-la     

        pthread_mutex_lock(&lock_corda);                //obtem a corda           
        pthread_mutex_lock(&lock_controle_vez);             //entra na região crítica de acesso às variáveis quer e vez.
            vez = GBA;                                  //sinaliza que a vez é sua
            quer[GBA] = 0;                              //sinaliza que não há gorilas esperando para acessar a corda deste lado
        pthread_mutex_unlock(&lock_controle_vez);           //sai da região crítica de acesso às variáveis quer e vez.
        pthread_mutex_unlock(&lock_trocar);             //A corda já foi obtida, lock_trocar pode ser cedida

        printf("Gorila passado de B para A \n");
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        sleep(20);

        //Procedimentos para quando sair da corda
        
        pthread_mutex_unlock(&lock_corda);              //cede o uso da corda
    }
    pthread_exit(0);
}
*/
int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
            if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
                printf("Não pode criar a thread %d\n", i);
                return -1;
            }
        }else{
            if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
                printf("Não pode criar a thread %d\n", i);
                return -1;
            }
        }
    }/*
    pthread_t gab;
    pthread_create(&gab, NULL, &gorilaAB, NULL);
    pthread_t gba;
    pthread_create(&gba, NULL, &gorilaBA, NULL);*/
    pthread_join(macacos[0], NULL);
    return 0;
}
