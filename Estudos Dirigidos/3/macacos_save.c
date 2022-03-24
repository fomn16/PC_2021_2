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

//lock responsável pelo uso da corda
pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;

//locks que restringem o acesso ao array num_macacos (1 para cada posição)
pthread_mutex_t lock_nm[2] = {PTHREAD_MUTEX_INITIALIZER};

//lock usado para impedir que novos macacos entrem na corda
//caso outro grupo - gorila ou outros macacos - queiram atravessar
pthread_mutex_t lock_trocar = PTHREAD_MUTEX_INITIALIZER;

//lock que restringe o acesso às variávis num_macacos e vez
pthread_mutex_t lock_quer_vez = PTHREAD_MUTEX_INITIALIZER;

int num_macacos[2] = {0};
int quer[4] = {0};
int vez = AB;

void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
        sleep(rand() % 10);                             //macacos chegam de tempos em tempos para atravessar

        //Procedimentos para acessar a corda
        int c = 0;                                      //variável de controle temporária, usada para impedir deadlocks

        //esta parte do código verifica se a lock_trocar deve ser obtida.
        //ela será obtida se a variável temporária c for setada (c = 1).
        //a lógica para setar ou não a variável é a seguinte:
        //Se o macaco pertence ao grupo que já está usando a corda, a variável
        //deve ser setada, pois ele deve ser impedido de acessar a corda caso
        //algum outro grupo queira usá-la.
        //Se não, c deve ser setada apenas se há macacos o suficiente (neste caso 5)
        //querendo usar a corda para o outro lado. A checagem quer[AB] != -1 ocorre
        //para impedir que o mesmo grupo de macacos use o lock_trocar várias vezes
        //(apenas 1 macaco irá tentar obter o lock_trocar, o resto firacá preso no comando
        //pthread_mutex_lock(&lock_corda))

        pthread_mutex_lock(&lock_quer_vez);             //entrando na região crítica de acesso às variáveis quer e vez.
        if(vez == AB)
            c = 1;
        else if(quer[AB] != -1){
            quer[AB]++;
            if(quer[AB] > 5){
                c = 1;
                quer[AB] = -1;
            }
        }
        pthread_mutex_unlock(&lock_quer_vez);           //saindo da região crítica de acesso às variáveis quer e vez.

        if(c)
            pthread_mutex_lock(&lock_trocar);           //verifica se algum outro grupo quer usar a corda, ou 
                                                        //sinaliza a vontade de usa-la

        pthread_mutex_lock(&lock_nm[AB]);               //lock é obtida para acesso à array num_macacos na posição AB 

        num_macacos[AB]++;                              //número de macacos usando a corda é incrementado 

        if(num_macacos[AB] == 1){                       //se este é o primeiro macaco deste grupo a acessar a corda

            pthread_mutex_lock(&lock_corda);            //obtem a corda para o grupo
        }
        pthread_mutex_lock(&lock_quer_vez);         //entra na região crítica de acesso às variáveis quer e vez.
                vez = AB;                               //sinaliza que a vez é de seu grupo
                quer[AB] = 0;                           //sinaliza que não há macacos de seu grupo esperando para acessar a corda
        pthread_mutex_unlock(&lock_quer_vez);       //sai da região crítica de acesso às variáveis quer e vez.
        
        pthread_mutex_unlock(&lock_trocar);             //A corda já foi obtida, lock_trocar pode ser cedida
        pthread_mutex_unlock(&lock_nm[AB]);             //lock para acesso à array num_macacos na posição AB é cedida 

        printf("Macaco %d passado de A para B \n",i);
        
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        sleep(5);

        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_nm[AB]);               //lock é obtida para acesso à array num_macacos na posição AB    
            num_macacos[AB]--;                          //número de macacos usando a corda é decrementado 
            if(num_macacos[AB] == 0){                   //se este é o último macaco deste grupo a acessar a corda
                pthread_mutex_unlock(&lock_corda);      //cede o uso da corda
            }
        pthread_mutex_unlock(&lock_nm[AB]);             //lock para acesso à array num_macacos na posição AB é cedida
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
        sleep(rand() % 10);                             //macacos chegam de tempos em tempos para atravessar

        ///Procedimentos para acessar a corda
        int c = 0;                                      //variável de controle temporária, usada para impedir deadlocks

        //esta parte do código verifica se a lock_trocar deve ser obtida.
        //ela será obtida se a variável temporária c for setada (c = 1).
        //a lógica para setar ou não a variável é a seguinte:
        //Se o macaco pertence ao grupo que já está usando a corda, a variável
        //deve ser setada, pois ele deve ser impedido de acessar a corda caso
        //algum outro grupo queira usá-la.
        //Se não, c deve ser setada apenas se há macacos o suficiente (neste caso 5)
        //querendo usar a corda para o outro lado. A checagem quer[BA] != -1 ocorre
        //para impedir que o mesmo grupo de macacos use o lock_trocar várias vezes
        //(apenas 1 macaco irá tentar obter o lock_trocar, o resto firacá preso no comando
        //pthread_mutex_lock(&lock_corda))

        pthread_mutex_lock(&lock_quer_vez);             //entrando na região crítica de acesso às variáveis quer e vez.
        if(vez == BA)
            c = 1;
        else if(quer[BA] != -1){
            quer[BA]++;
            if(quer[BA] > 5){
                c = 1;
                quer[BA] = -1;
            }
        }
        pthread_mutex_unlock(&lock_quer_vez);           //saindo da região crítica de acesso às variáveis quer e vez.

        if(c)
            pthread_mutex_lock(&lock_trocar);           //verifica se algum outro grupo quer usar a corda, ou 
                                                        //sinaliza a vontade de usa-la

        pthread_mutex_lock(&lock_nm[BA]);               //lock é obtida para acesso à array num_macacos na posição BA 

        num_macacos[BA]++;                              //número de macacos usando a corda é incrementado 

        if(num_macacos[BA] == 1){                       //se este é o primeiro macaco deste grupo a acessar a corda

            pthread_mutex_lock(&lock_corda);            //obtem a corda para o grupo
        }
        pthread_mutex_lock(&lock_quer_vez);         //entra na região crítica de acesso às variáveis quer e vez.
                vez = BA;                               //sinaliza que a vez é de seu grupo
                quer[BA] = 0;                           //sinaliza que não há macacos de seu grupo esperando para acessar a corda
        pthread_mutex_unlock(&lock_quer_vez);       //sai da região crítica de acesso às variáveis quer e vez.
        
        pthread_mutex_unlock(&lock_trocar);             //A corda já foi obtida, lock_trocar pode ser cedida
        pthread_mutex_unlock(&lock_nm[BA]);             //lock para acesso à array num_macacos na posição BA é cedida  

        printf("Macaco %d passado de B para A \n",i);
        
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        sleep(5);
        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_nm[BA]);               //lock é obtida para acesso à array num_macacos na posição BA    
            num_macacos[BA]--;                          //número de macacos usando a corda é decrementado 
            if(num_macacos[BA] == 0){                   //se este é o último macaco deste grupo a acessar a corda
                pthread_mutex_unlock(&lock_corda);      //cede o uso da corda
            }
        pthread_mutex_unlock(&lock_nm[BA]);             //lock para acesso à array num_macacos na posição BA é cedida
    }
    pthread_exit(0);
}

void * gorilaAB(void * a){
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
        //Se não, c deve ser setada. A checagem quer[GAB] != -1 ocorre
        //para impedir que o mesmo grupo use o lock_trocar várias vezes
        //(apenas 1 gorila nessa direcao irá tentar obter o lock_trocar, o resto firacá preso no comando
        //pthread_mutex_lock(&lock_corda))
        pthread_mutex_lock(&lock_quer_vez);             //entrando na região crítica de acesso às variáveis quer e vez.
        if(vez == GAB)
            c = 1;
        else if(quer[GAB] != -1){
            c = 1;
            quer[GAB] = -1;
        }
        pthread_mutex_unlock(&lock_quer_vez);           //saindo da região crítica de acesso às variáveis quer e vez.
        if(c)
            pthread_mutex_lock(&lock_trocar);           //verifica se algum outro grupo quer usar a corda, ou 
                                                        //sinaliza a vontade de usa-la     

        pthread_mutex_lock(&lock_corda);                //obtem a corda           
        pthread_mutex_lock(&lock_quer_vez);             //entra na região crítica de acesso às variáveis quer e vez.
            vez = GAB;                                  //sinaliza que a vez é sua
            quer[GAB] = 0;                              //sinaliza que não há gorilas esperando para acessar a corda deste lado
        pthread_mutex_unlock(&lock_quer_vez);           //sai da região crítica de acesso às variáveis quer e vez.
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
        pthread_mutex_lock(&lock_quer_vez);             //entrando na região crítica de acesso às variáveis quer e vez.
        if(vez == GBA)
            c = 1;
        else if(quer[GBA] != -1){
            c = 1;
            quer[GBA] = -1;
        }
        pthread_mutex_unlock(&lock_quer_vez);           //saindo da região crítica de acesso às variáveis quer e vez.
        if(c)
            pthread_mutex_lock(&lock_trocar);           //verifica se algum outro grupo quer usar a corda, ou 
                                                        //sinaliza a vontade de usa-la     

        pthread_mutex_lock(&lock_corda);                //obtem a corda           
        pthread_mutex_lock(&lock_quer_vez);             //entra na região crítica de acesso às variáveis quer e vez.
            vez = GBA;                                  //sinaliza que a vez é sua
            quer[GBA] = 0;                              //sinaliza que não há gorilas esperando para acessar a corda deste lado
        pthread_mutex_unlock(&lock_quer_vez);           //sai da região crítica de acesso às variáveis quer e vez.
        pthread_mutex_unlock(&lock_trocar);             //A corda já foi obtida, lock_trocar pode ser cedida

        printf("Gorila passado de B para A \n");
        printf("%d, %d, %d, %d\n", quer[0], quer[1], quer[2], quer[3]);
        sleep(20);

        //Procedimentos para quando sair da corda
        
        pthread_mutex_unlock(&lock_corda);              //cede o uso da corda
    }
    pthread_exit(0);
}

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
    }
    pthread_t gab;
    pthread_create(&gab, NULL, &gorilaAB, NULL);
    pthread_t gba;
    pthread_create(&gba, NULL, &gorilaBA, NULL);
    pthread_join(macacos[0], NULL);
    return 0;
}
