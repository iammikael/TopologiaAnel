#include <stdio.h>
#include <mpi.h> //importando bibioteca mpi que iremos usar

int main(int argc, char *argv[]){

    // variavel rank responsavel por identificar cada processo
    int rank; 
    int tamanho;

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

    
    MPI_Comm_create(MPI_COMM_WORLD, novo_grupo, &novo_comunicador);

    MPI_Comm_rank(novo_comunicador, &rank_comunicador);

    MPI_Group_rank(novo_grupo, &rank_grupo);

    printf("Rank global: %d - Rank no grupo: %d - rank comunicador: %d\n", rank, rank_grupo, rank_comunicador);


    if(rank < 4){
        int valor =10;
            if(rank_comunicador == 0){
                MPI_Send(&valor, 1, MPI_INT, 1, 0, novo_comunicador);
                printf("Processo 0 enviou %d para P1\n", valor);

                MPI_Recv(&valor, 1, MPI_INT, 3, 0,novo_comunicador,MPI_STATUS_IGNORE);
                printf("Processo 0 recebeu o valor final do anel: %d \n", valor);

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

    if(rank >= 4){
        int valor =20;

            //Aqui iniciamos o processo P4 no rank local 0
            if(rank_comunicador == 0){
                MPI_Send(&valor, 1, MPI_INT, 3, 0, novo_comunicador);
                printf("Processo 0 enviou %d para P7\n", valor);

                MPI_Recv(&valor, 1, MPI_INT, 1, 0,novo_comunicador,MPI_STATUS_IGNORE);
                printf("Processo p4 recebeu o valor final do anel: %d \n", valor);

                MPI_Send(&valor,1,MPI_INT, 0, 10,MPI_COMM_WORLD);

            //processo P3 no rank local 3
            }else if(rank_comunicador == 3){
                MPI_Recv(&valor, 1, MPI_INT, 0, 0,novo_comunicador,MPI_STATUS_IGNORE);
                valor = valor - 1;

                printf("Processo p7 recebeu e decrementou o valor para %d\n",valor);
                MPI_Send(&valor,1, MPI_INT, 2,0,novo_comunicador);

            //processo P4 no rank local 4
            }else if(rank_comunicador == 2){
                MPI_Recv(&valor,1,MPI_INT, 3, 0, novo_comunicador, MPI_STATUS_IGNORE);
                valor = valor - 1;

                printf("Processo p6 recebeu e decrementou o valor para %d\n",valor);
                MPI_Send(&valor,1, MPI_INT, 1,0,novo_comunicador);

            //processo P5 no rank local 5
            }else if(rank_comunicador == 1){
                MPI_Recv(&valor,1,MPI_INT, 2, 0,novo_comunicador, MPI_STATUS_IGNORE);
                valor = valor - 1;

                printf("Processo p5 recebeu e decrementou o valor para %d\n",valor);
                MPI_Send(&valor,1, MPI_INT, 0,0,novo_comunicador);
            }
    }

    MPI_Comm_free(&novo_comunicador);

    MPI_Group_free(&novo_grupo);
    MPI_Group_free(&grupo_original);
    MPI_Finalize();
    
    return 0;
}