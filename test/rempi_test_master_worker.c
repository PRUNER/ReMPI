
#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>


#include "rempi_test_util.h"

#define NUM_MEG_PER_RANK (1000)
#define MAX_HASH (1000000)
int my_rank;


int main(int argc, char *argv[])
{

  int rank, size;
  MPI_Status status;
  double start, end, overall_end;
  int hash = 0;


  /* Init */
  MPI_Init(&argc, &argv);
  start = MPI_Wtime();
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  init_rand(rank);

  int k = 0;
  for (k=0; k<1; k++) {
    if (rank != 0) { // Slaves
      int buf;
      int i;
      for (i = 0; i < NUM_MEG_PER_RANK; i++) {
	/*Emulate two wave of MPI_Send, so that rank=0 can poll MPI_Test to wait the MPI_Send waves*/
	//      usleep(100);
	MPI_Send(&rank, 1, MPI_INT, 0, i, MPI_COMM_WORLD); 
	//      fprintf(stderr, "send\n");
      }
    } else { // Master
      int sum = 0;
      int flag = -1, res;
      MPI_Request request;
      MPI_Status status;
      rempi_test_dbg_print("Start: %f", MPI_Wtime());
      while (sum < (size - 1) * NUM_MEG_PER_RANK) { 
	if(flag == -1) {
	  MPI_Irecv(&res, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
	  flag = 0;
	}
	MPI_Test(&request, &flag, &status);
	if (sum == 0) {
	  rempi_test_dbg_print("First MPI_Test Done: %f", MPI_Wtime());
	}
	//      fprintf(stderr, "flag 1: %d (%d %d)\n", flag, status.MPI_SOURCE, status.MPI_TAG);
	//      MPI_Test(&request, &flag, &status);
	//      fprintf(stderr, "flag 3: %d (%d %d)\n", flag, status.MPI_SOURCE, status.MPI_TAG);
	/* MPI_Wait(&request, &status); */
	/* fprintf(stderr, "wait       (%d %d)\n",  status.MPI_SOURCE, status.MPI_TAG); */
	/* MPI_Wait(&request, &status); */
	/* fprintf(stderr, "wait       (%d %d)\n",  status.MPI_SOURCE, status.MPI_TAG); */

	if (flag == 1) { 
	  //	  rempi_test_dbg_print("** Slave ID: %d ** (time: %f)", status.MPI_SOURCE, MPI_Wtime());
	  if (sum == 0) {
	    rempi_test_dbg_print("First Matched: %f", MPI_Wtime());
	  }
	  if (status.MPI_SOURCE != -1) {
	    sum++;
	  }
	  hash = get_hash(hash * (status.MPI_SOURCE + 10) * 10, MAX_HASH);
	  flag = -1;
	}
      }
    }
  }

  end = MPI_Wtime();
  MPI_Finalize();
  //  overall_end = MPI_Wtime();
  if (rank == 0) {
    //    fprintf(stdout, "Hash %d, Time (Main loop): %f, Time (Overall): %f\n", hash, end - start, overall_end - start);
    fprintf(stdout, "Hash %d, Time (Main loop): %f\n", hash, end - start);
  }
  return 0;

}
