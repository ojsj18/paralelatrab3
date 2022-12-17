#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void main(int argc, char* argv[]) {

  int size, rank;
  long elements;
  long nMsg;
  long tMsg;
  long ni;    

  long msgS;
  long msgR;

  MPI_Status stat;

  MPI_Init(&argc, &argv) ;
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(size > 2){
    printf("só são permitidos 2 processos \n");
    exit(0);
  }

  if (argc != 3)
	{
		printf("usage: %s <nmsg> <tmsg>\n",argv[0]);
		exit(0);
	}
	else
	{
		nMsg = atoi(argv[1]);
  	printf("<nmsg>: %ld \n",nMsg);

		if (nMsg%2 != 0)
		{
			printf("<nmsg>: %ld (precisa ser um numero par!!)\n",nMsg);
			exit(0);
		}
    tMsg = atoi(argv[2]);
    if (tMsg%sizeof(long) != 0)
		{
			printf("<tmsg>: %ld (precisa ser um numero multiplo de 8!!)\n",tMsg);
			exit(0);
		}
    ni = tMsg/8;

	}

  long *ping = (long *) malloc( nMsg*sizeof(long) );
  long *pong = (long *) malloc( nMsg*sizeof(long) );

  MPI_Request reqs[nMsg];//???
	MPI_Status stats[nMsg];

  if(size > 2){
    exit(0);
  }
  
  if (rank == 0) {
      elements = 1;
      for(int i=0;i<nMsg/2;i++){
        ping[i] = elements;
        elements+= 1;
      }
    printf(" %ld %ld \n",nMsg, ni);
    for(int i=0;i<nMsg/2;i+=ni){
      MPI_Isend(&ping[i], ni, MPI_LONG, 1, 0, MPI_COMM_WORLD, &reqs[i]);
      printf("ping: enviado a %d msg\n",i);
      MPI_Irecv(&pong[i], ni, MPI_LONG, 1, 0, MPI_COMM_WORLD, &reqs[i+1]);
      printf("ping: recebido a %d msg\n",i);
    }
      printf("vetor pong recebido: ");
      for(int i = 0;i<nMsg/2;i++){
        printf("%ld ",pong[i]);
      }
      printf("\n");

  } else if (rank == 1) {
      elements = (nMsg/2) + 1;
      for(int i=0;i<nMsg/2;i++){
        pong[i] = elements;
        elements+= 1;
      }
      printf(" %ld %ld \n",nMsg, ni);
      for(int i=0;i<nMsg/2;i+=ni){
        MPI_Irecv(&ping[i], ni, MPI_LONG, 0, 1, MPI_COMM_WORLD, &reqs[i]);

        printf("pong: recebido a %d msg\n",i);
        MPI_Isend(&pong[i], ni, MPI_LONG, 0, 1, MPI_COMM_WORLD, &reqs[i+1]);

        printf("pong: enviado a %d msg\n",i);
      }
      printf("vetor ping recebido: ");
      for(int i = 0;i<nMsg/2;i++){
        printf("%ld ",ping[i]);
      }
      printf("\n");
  }
  
  MPI_Finalize();    
}