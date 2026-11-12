//Trabalho de sistema distribuidos - Topologia em anel 
//Nomes: Mikael Bueno e Mayla Escurra 
// Nesse trabalho para compilar usei o seguinte comando no terminal linux -> mpicc anel.c -o anel
//E para executar -> mpirun -np 8 ./anel


#include <stdio.h>
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

    int ranks1[4] = {0,1,2,3}; //processo do anel horario
    int ranks2[4] = {4,5,6,7}; //processo do anel anti horario


    //inicializo o MPI
    MPI_Init(&argc,&argv);

    //Com esses comandos eu descubro o identificado dos processos e a quantidade de processos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanho);

    MPI_Comm_group(MPI_COMM_WORLD,&grupo_original); // grupo onde contem todos os processos

    if(rank < 4){
        MPI_Group_incl(grupo_original, 4, ranks1, &novo_grupo);
    }else{
        MPI_Group_incl(grupo_original, 4, ranks2, &novo_grupo);
    }

    //Criacao de um comunicador para cada grupo de processo
    MPI_Comm_create(MPI_COMM_WORLD, novo_grupo, &novo_comunicador);

    MPI_Comm_rank(novo_comunicador, &rank_comunicador);

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
            //inicio do anel do sentido horario
            if(rank < 4){
                int valor = x;
                    if(rank_comunicador == 0){
                        MPI_Send(&valor, 1, MPI_INT, 1, 0, novo_comunicador);
                        printf("Processo 0 enviou %d para P1\n", valor);

                        MPI_Recv(&valor, 1, MPI_INT, 3, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        printf("Processo 0 recebeu o valor final do anel: %d \n", valor);

                        x = valor;

                        int antiHorario;

                        MPI_Recv(&antiHorario,1,MPI_INT, 4,10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        printf("\n-- Resultado dos aneis --\n");
                        printf("Anel horario %d\n",valor);
                        printf("Anel anti-horario %d\n",antiHorario);

                    }else if(rank_comunicador == 1){
                        MPI_Recv(&valor, 1, MPI_INT, 0, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        valor = valor + 1;

                        printf("Processo 1 recebeu e incrementou o valor para %d\n",valor);
                        MPI_Send(&valor,1, MPI_INT, 2,0,novo_comunicador);

                    }else if(rank_comunicador == 2){
                        MPI_Recv(&valor,1,MPI_INT, 1, 0, novo_comunicador, MPI_STATUS_IGNORE);
                        valor = valor + 1;

                        printf("Processo 2 recebeu e incrementou o valor para %d\n",valor);
                        MPI_Send(&valor,1, MPI_INT, 3,0,novo_comunicador);

                    }else if(rank_comunicador == 3){
                        MPI_Recv(&valor,1,MPI_INT, 2, 0,novo_comunicador, MPI_STATUS_IGNORE);
                        valor = valor + 1;

                        printf("Processo 3 recebeu e incrementou o valor para %d\n",valor);
                        MPI_Send(&valor,1, MPI_INT, 0,0,novo_comunicador);
                    }
            }

            //inicio do anel do sentido anti-horario
            if(rank >= 4){
                int valor =y;

                    //Aqui iniciamos o processo P4 no rank local 0
                    if(rank_comunicador == 0){
                        MPI_Send(&valor, 1, MPI_INT, 3, 0, novo_comunicador);
                        printf("Processo 4 enviou %d para P7\n", valor);

                        MPI_Recv(&valor, 1, MPI_INT, 1, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        printf("Processo 4 recebeu o valor final do anel: %d \n", valor);

                        y = valor;

                        MPI_Send(&valor,1,MPI_INT, 0, 10,MPI_COMM_WORLD);

                    //processo P7 no rank local 3
                    }else if(rank_comunicador == 3){
                        MPI_Recv(&valor, 1, MPI_INT, 0, 0,novo_comunicador,MPI_STATUS_IGNORE);
                        valor = valor - 1;

                        printf("Processo 7 recebeu e decrementou o valor para %d\n",valor);
                        MPI_Send(&valor,1, MPI_INT, 2,0,novo_comunicador);

                    //processo P6 no rank local 2
                    }else if(rank_comunicador == 2){
                        MPI_Recv(&valor,1,MPI_INT, 3, 0, novo_comunicador, MPI_STATUS_IGNORE);
                        valor = valor - 1;

                        printf("Processo 6 recebeu e decrementou o valor para %d\n",valor);
                        MPI_Send(&valor,1, MPI_INT, 1,0,novo_comunicador);

                    //processo P5 no rank local 1
                    }else if(rank_comunicador == 1){
                        MPI_Recv(&valor,1,MPI_INT, 2, 0,novo_comunicador, MPI_STATUS_IGNORE);
                        valor = valor - 1;

                        printf("Processo 5 recebeu e decrementou o valor para %d\n",valor);
                        MPI_Send(&valor,1, MPI_INT, 0,0,novo_comunicador);
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
