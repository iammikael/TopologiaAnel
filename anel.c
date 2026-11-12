//Trabalho de sistema distribuidos - Topologia em anel 
//Nomes: Mikael Bueno e Mayla Escurra 
// Nesse trabalho para compilar usei o seguinte comando no terminal linux -> mpicc anel.c -o anel
//E para executar -> mpirun -np 8 ./anel


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> //importando bibioteca mpi que iremos usar

int main(int argc, char *argv[]){

    // variavel rank responsavel por identificar cada processo
    int rank; 
    int tamanho;
    int opcao = -1;
    int x;
    int y;

    MPI_Group grupo_original;
    MPI_Group novo_grupo;
    MPI_Comm novo_comunicador;

    int rank_comunicador;
    int rank_grupo;

    //inicializo o MPI
    MPI_Init(&argc,&argv);

    //Com esses comandos eu descubro o identificado dos processos e a quantidade de processos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanho);

    if(tamanho<4 || tamanho%2 != 0){
        if(rank == 0)
        printf("Inicie com um numero par de processos \n");
        MPI_Finalize();
        return 1;
    }

    int metade = tamanho/2;
    int *ranks1 = malloc(metade * sizeof(int));
    int *ranks2 = malloc(metade * sizeof(int));

    for (int i=0; i<metade; i++){
        ranks1[i]=i;
        ranks2[i]= metade + i;
    }

    MPI_Comm_group(MPI_COMM_WORLD,&grupo_original); // grupo onde contem todos os processos

    // nesse if eu verifico qual a quantidade processos que o usuario iniciou e faço a divisão
    if(rank < metade){
        MPI_Group_incl(grupo_original, metade, ranks1, &novo_grupo);
    }else{
        MPI_Group_incl(grupo_original, metade, ranks2, &novo_grupo);
    }

    //Criacao de um comunicador para cada grupo de processo
    MPI_Comm_create(MPI_COMM_WORLD, novo_grupo, &novo_comunicador);

    MPI_Comm_rank(novo_comunicador, &rank_comunicador);

    //defino as variaveis para avançar e voltar entre os processos
    int prox = (rank_comunicador + 1) % metade;
    int ant = (rank_comunicador - 1 + metade) % metade;

    MPI_Group_rank(novo_grupo, &rank_grupo);

    if(rank == 0){
        printf("\nInforme o valor inicial de x:");
        fflush(stdout);
        scanf("%d",&x);
        
        printf("\nInforme o valor inicial de y:");
        fflush(stdout);
        scanf("%d",&y);
        printf("\n");
    }

    MPI_Bcast(&x,1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y,1, MPI_INT, 0, MPI_COMM_WORLD);

    while(opcao !=3){
        int valor;
            //inicio do anel do sentido horario
            if(rank < metade){
                    valor = x;
                    if(rank_comunicador == 0){
                        MPI_Send(&valor, 1, MPI_INT, prox, 0, novo_comunicador);
                        printf("Processo %d enviou %d para P%d\n",rank, valor, prox);

                        MPI_Recv(&valor, 1, MPI_INT, ant, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        printf("Processo %d recebeu o valor final do anel: %d \n",rank, valor);

                        x = valor;

                        int antiHorario;
                        MPI_Recv(&antiHorario,1,MPI_INT, metade, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        printf("\n-- Resultado dos aneis --\n");
                        printf("Anel horario %d\n",valor);
                        printf("Anel anti-horario %d\n",antiHorario);

                    }else{
                      MPI_Recv(&valor, 1, MPI_INT, ant, 0, novo_comunicador, MPI_STATUS_IGNORE);
                      valor= valor + 1;

                      printf("Processo %d recebeu e incrementou o valor para %d\n",rank, valor);
                      MPI_Send(&valor, 1, MPI_INT, prox, 0, novo_comunicador);
                    }
            }

            //inicio do anel do sentido anti-horario
            if(rank >= metade){
                    //Aqui iniciamos o processo P4 no rank local 0
                    if(rank_comunicador == 0){
                        valor =y;

                        MPI_Send(&valor, 1, MPI_INT, ant, 0, novo_comunicador);
                        printf("Processo %d enviou %d para P%d\n",rank, valor, metade + ant);

                        MPI_Recv(&valor, 1, MPI_INT, prox, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        printf("Processo %d recebeu o valor final do anel: %d \n",rank, valor);

                        y = valor;
                        MPI_Send(&valor,1,MPI_INT, 0, 10,MPI_COMM_WORLD);

                    }else {
                        MPI_Recv(&valor, 1, MPI_INT, prox, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        valor = valor - 1;

                        printf("Processo %d recebeu e decrementou o valor para %d\n",rank, valor);
                        MPI_Send(&valor,1, MPI_INT, ant,0,novo_comunicador);

                    }
            }

                if(rank == 0){
                    printf("\n----MENU----\n");
                    printf("1 - Continuar a comunicao \n");
                    printf("2 - Informar novos valores\n");
                    printf("3 - Encerrar\n");

                    printf("\nEscolha uma opcao -> ");
                    fflush(stdout);
                    scanf("%d",&opcao);

                
                }

                //Usando o MPI Bcast eu envio para todos os processos a opcao
                // que o usuario escolheu
                MPI_Bcast(&opcao, 1, MPI_INT,0, MPI_COMM_WORLD);

                if(opcao == 2){
                    if(rank == 0){
                        printf("Informe o valor inicial de x:");
                        fflush(stdout);
                        scanf("%d",&x);
                        
                        printf("Informe o valor inicial de y:");
                        fflush(stdout);
                        scanf("%d",&y);
                    }

                    //Aqui eu usei o MPI Bcast para poder compartilhar com todos os processos
                    //os valores escolhidos pelo usuario
                    MPI_Bcast(&x,1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast(&y,1, MPI_INT, 0, MPI_COMM_WORLD);
                }

    }

    printf("Processo %d recebeu a opcao %d\n", rank,opcao);

    MPI_Comm_free(&novo_comunicador);

    MPI_Group_free(&novo_grupo);
    MPI_Group_free(&grupo_original);
    MPI_Finalize();
    
    return 0;
}
