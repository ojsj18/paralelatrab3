#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DEBUG   1

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

  long *ping = (long *) malloc( ni*sizeof(long) );
  long *pong = (long *) malloc( ni*sizeof(long) );

  MPI_Request reqsSend;
  MPI_Request reqsRec;
  MPI_Status status;

  if(size > 2){
    exit(0);
  }
  
  if (rank == 0)
      for(int i=0;i<ni;i++) {
        ping[i] = i+1;
        pong[i] = 0;
      }  

  else if (rank == 1)
      for(int i=0;i<ni;i++) {
            ping[i] = 0;
            pong[i] = i+ni+1;
      }   
            
  #if DEBUG
        fprintf(stderr, "\nrank: %d Ping[ ", rank );
        for(int i=0;i<ni;i++)
             fprintf(stderr, "%ld ", ping[i] );
        fprintf(stderr, " ]\n" );

        fprintf(stderr, "rank: %d Pong[ ", rank );
        for(int i=0;i<ni;i++)
             fprintf(stderr, "%ld ", pong[i] );
        fprintf(stderr, " ]\n" );
  #endif          
  
  for(int i=0;i<nMsg/2;i++)
      
      if (rank == 0) {
        MPI_Isend(ping, ni, MPI_LONG, 1, i, MPI_COMM_WORLD, &reqsSend);
        printf("rank %d enviou ping %d ao nodo %d\n", rank, i, 1);
        MPI_Irecv(pong, ni, MPI_LONG, 1, i, MPI_COMM_WORLD, &reqsRec);
        printf("rank %d requisitou recepcao de pong %d do nodo %d\n", rank, i, 1);

        MPI_Wait( &reqsRec, &status);

      } else if (rank == 1) {
        MPI_Isend(pong, ni, MPI_LONG, 0, i, MPI_COMM_WORLD, &reqsSend);
        printf("rank %d enviou ping %d ao nodo %d\n", rank, i, 0);
        MPI_Irecv(ping, ni, MPI_LONG, 0, i, MPI_COMM_WORLD, &reqsRec);
        printf("rank %d requisitou recepcao de pong %d do nodo %d\n",rank, i, 0);

        MPI_Wait( &reqsRec, &status);
    }

  MPI_Finalize();
}
