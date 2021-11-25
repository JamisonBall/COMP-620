/********************************************************************************
@author: Jamison Ball
@date: 11/25/2021
********************************************************************************/
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int size;
    int rank;
    int data_set[16];
    int target = 5; //the thing we're searching for
    int i; //iterator
    int process_chunk[8];
    
    //init dataset
    for(i = 0; i < 16; i++) {
        data_set[i] = i;
    }
    //set up parallel environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    MPI_Scatter(&data_set, 8, MPI_INT, &process_chunk, 8, MPI_INT, 0,      MPI_COMM_WORLD);
    
    if(rank == 0) { //MAKE EXPLICITLY SURE THAT PROCESSES RUN IN PARALLEL
       int j; //iterator
       
       for(j = 0; j < 8; j++) {
           if(process_chunk[j] == target) {
               printf("Process %d was able to find %d\n", rank, target);
               break;
           }
       }
       
       if(j == 8) {
           printf("Process %d was unable to find %d\n", rank, target);
       }
    } else if (rank != 0) {
        int k; //iterator
        
        for(k = 0; k < 8; k++) {
           if(process_chunk[k] == target) {
               printf("Process %d was able to find %d\n", rank, target);
               break;
           }
       }
       
       if(k == 8) {
           printf("Process %d was unable to find %d\n", rank, target);
       }
    }
    MPI_Finalize();
    return 0;
}
