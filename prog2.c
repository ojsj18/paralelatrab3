#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define TAG 1

void main(int argc, char* argv[]) {
  int size, rank;
  long number;    
  MPI_Status stat;

  long ping[];
  long pong[];
  //preenche o ping se for o 1
  //preenche o pong se for o 2 
  MPI_Init(&argc, &argv) ;
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  if(size > 2){
    exit(0);
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (rank == 0) {
    printf("Processor 0 sends a message to 1\n");
    number = 123456789 ;
    MPI_Send(&number, 1, MPI_LONG, 1, TAG, MPI_COMM_WORLD);
  
  } else if (rank == 1) {
    printf("Processor 1 receives a message from 0\n");
    MPI_Recv(&number, 1, MPI_LONG, 0, TAG, MPI_COMM_WORLD, &stat);
    printf("I received the value: %ld \n", number);
  }
  
  MPI_Finalize();    
}