#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "chrono.c"

int main(int argc, char* argv[]) {

  chronometer_t mpiTime;

  int size, rank;
  long elements;
  long nMsg;
  long tMsg;
  long ni;


  long msgS;
  long msgR;
  int bloqueante = 0;

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
		printf("usage: %s <nmsg> <tmsg> <Optional: bl|nbl>\n",argv[0]);
		exit(0);
	}
	else
	{
    
		nMsg = atoi(argv[1]);
  	printf("<nmsg>: %ld \n",nMsg);

    if(argc == 4)
      bloqueante =  0; strcmp(argv[3], "-nbl");

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
  
  chrono_reset(&mpiTime);
	chrono_start(&mpiTime);

  if(!bloqueante){ // se == 0, nbloqueante
      printf("NAO BLOQUEANTE\n");
      for(int i=0;i<nMsg/2;i++)
        if (rank == 0) {
          MPI_Isend(ping, ni, MPI_LONG, 1, i, MPI_COMM_WORLD, &reqsSend);
          MPI_Irecv(pong, ni, MPI_LONG, 1, i, MPI_COMM_WORLD, &reqsRec);

          MPI_Wait( &reqsRec, &status);

        } else if (rank == 1) {
          MPI_Isend(pong, ni, MPI_LONG, 0, i, MPI_COMM_WORLD, &reqsSend);
          MPI_Irecv(ping, ni, MPI_LONG, 0, i, MPI_COMM_WORLD, &reqsRec);

          MPI_Wait( &reqsRec, &status);
      }
  } else{         //se != 0 bloqueante
    printf("BLOQUEANTE\n");
    for(int i=0;i<nMsg/2;i++)
        if (rank == 0) {
          MPI_Send(ping, ni, MPI_LONG, 1, i, MPI_COMM_WORLD);
          MPI_Recv(pong, ni, MPI_LONG, 1, i, MPI_COMM_WORLD, &status);

        } else if (rank == 1) {
          MPI_Send(pong, ni, MPI_LONG, 0, i, MPI_COMM_WORLD);
          MPI_Recv(ping, ni, MPI_LONG, 0, i, MPI_COMM_WORLD, &status);
        }
  }

  MPI_Barrier( MPI_COMM_WORLD );

  chrono_stop(&mpiTime);

	double total_time_in_seconds = (double)chrono_gettotal(&mpiTime) /
								   ((double)1000 * 1000 * 1000);
  printf("total: %lf vazao\n", total_time_in_seconds);

  double OPS = total_time_in_seconds / (nMsg) ;
	printf("vazao: %lf vazao\n", OPS);


	double mgb = ((nMsg * tMsg) / total_time_in_seconds)/ (1000*1000);
	printf("Throughput: %ld MB/s\n", mgb);

  MPI_Finalize();
}
