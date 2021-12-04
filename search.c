/********************************************************************************
@author: Jamison Ball
@date: 11/25/2021
@updated: 12/4/2021
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int init_buffer(int**, int);
int free_buffer(int**);
int search(int*, int);

int search(int *values, int target) {
	
	int i;
	
	for(i = 0; i < 8; i++) {
		if(values[i] == target) {
			return i;
		}
	}
	return -1;
}

int init_buffer(int **buf, int size) {
	int *mem = (int*)malloc(sizeof(int) * size);
	
	if(!mem) {
		return -1;
	}
	(*buf) = (int*)malloc(sizeof(int) * size);
	
	if(!(*buf)) {
		free(mem);
		return -1;
	}
	
	int i;
	
	buf[0] = &mem[0];
	return 0;
}

int free_buffer(int **buf) {
	free(&((*buf)[0]));
	return 0;
}
int main(int argc, char **argv) {
    int size;
    int rank;
    int target = 5; //the thing we're searching for
    int *data_set;
    int *process_chunk;
    time_t mpi_start;
    
    
    //set up parallel environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(rank == 0) {
    	mpi_start = clock();
    	printf("MPI started at: %ld\n", mpi_start);
    	int i;
    	init_buffer(&data_set, 16);
    	
    	for(i = 0; i < 16; i++) {
    		data_set[i] = i;
    	}
    }
    MPI_Scatter(data_set, 8, MPI_INT, process_chunk, 8,
    		MPI_INT, 0, MPI_COMM_WORLD);
    /*The scatter and search are both done in parallel since everything 	  outside of the rank check
      is executed by every process. MPI_Scatter will act as a recieve for
      every process but the sender (the second to last parameter.) 
      See https://www.mpich.org/static/docs/v3.1/www3/MPI_Scatter.html
      for more information*/
      int *results;
      
      init_buffer(&results, 2);
      
      
      
      if(rank == 0) {
      	int found_at_index = search(process_chunk, target);
      	results[0] = found_at_index;
      	MPI_Recv(&results[1], 1, MPI_INT, 1, 0,
      			 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      } else {
        int found_at_index = search(process_chunk, target);
      	MPI_Send(&found_at_index, 1, MPI_INT, 0, 0,
      			 MPI_COMM_WORLD);
      }
    
    
    
    
    //cleanup
    if(rank == 0) {
    	printf("Results:\n");
    	
    	if(results[0] == -1 && results[1] == -1) {
    		printf("Neither process was able to find the value %d. This value is not in the array.\n", target);
    	} else if(results[0] != -1 && results[1] != -1) {
    		printf("There are at least 2 occurances of the value %d. Both proccesses were able to find it.\n", target);
    		
    	} else if(results[0] != -1 && results[1] == -1) {
    		printf("Process 0 was able to find the value %d in subarray %d at index %d.\n", target, rank + 1, results[rank]);
    	} else {
    		printf("Process 1 was able to find the value %d in subarray %d at index %d.\n", target, rank + 2, results[rank + 1]);
    	}
    	printf("Program execution finished after: %ld\n", clock() - mpi_start);
    	free_buffer(&data_set);
    	free_buffer(&results);
    }
    MPI_Finalize();
}
