#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

//message tags
#define MASTER 0
#define SLAVE 1

int init_buffer(int***, int, int);
int free_buffer(int***);
int mul_add(int*, int*);
int *get_row(int**, int);
int *get_column(int**, int);

int main(int argc, char **argv) {
   int size;
   int rank;
   int **matrix_b;
   int **rows;
   
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   time_t mpi_start = clock();
   init_buffer(&matrix_b, 8, 8);
   init_buffer(&rows, 2, 8);
   
   if(rank == 0) {
   		int **matrix_a;
   		int **matrix_c;
   		
   		init_buffer(&matrix_a, 8, 8);
   		
   		for(int i = 0; i < 8; i++) {
   			for(int j = 0; j < 8; j++) {
   				matrix_a[i][j] = j;
   			}
   			
   			int row = 1;
   			for(int i = 0; i < 8; i++) {
   				for(int j = 0; j < 8; j++) {
   					matrix_b[i][j] = (j * 5) * row;
   				}
   				row++;
   			}
   		}
   		
   		printf("Matrix A:\n");
   		
   		for(int i = 0; i < 8; i++) {
   			for(int j = 0; j < 8; j++) {
   				printf("%d\t", matrix_a[i][j]);
   			}
   			printf("\n");
   		}
   		printf("Matrix B:\n");
   		
   		for(int i = 0; i < 8; i++) {
   			for(int j = 0; j < 8; j++) {
   				printf("%d\t", matrix_b[i][j]);
   			}
   			printf("\n");
   		}
   		
   		//take possession of first 2 rows
   		for(int k = 0; k < 2; k++) {
   			for(int m = 0; m < 8; m++) {
   				rows[k][m] = m;
   			}
   		}
   		//distribute the rest to the 3 other processes
   		MPI_Bcast(&(matrix_b[0][0]), 64, MPI_INT, 0, MPI_COMM_WORLD);
   		
   		MPI_Send(&(matrix_a[2][0]), 16, MPI_INT, 1, MASTER,
   				 MPI_COMM_WORLD);
   		MPI_Send(&(matrix_a[4][0]), 16, MPI_INT, 2, MASTER,
   				 MPI_COMM_WORLD);
   		MPI_Send(&(matrix_a[6][0]), 16, MPI_INT, 3, MASTER,
   				 MPI_COMM_WORLD);
   				 
   		init_buffer(&matrix_c, 8, 8);
   		
   		for(int i = 0; i < 2; i++) {
   			int *current_row = get_row(rows, i);
   			
   			for(int j = 0; j < 8; j++) {
   				int *current_column = get_column(matrix_b, j);
   				matrix_c[i][j] = mul_add(current_row, current_column);
   			}
   		}
   		MPI_Recv(&(matrix_c[2][0]), 16, MPI_INT, 1, SLAVE,
   				 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   		MPI_Recv(&(matrix_c[4][0]), 16, MPI_INT, 2, SLAVE,
   				 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   	        MPI_Recv(&(matrix_c[6][0]), 16, MPI_INT, 3, SLAVE,
   	    		         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   	    
   	    printf("Multiply finished in: %ld clock ticks.\nResult Matrix C:\n", clock() - mpi_start); 
   	    for(int k = 0; k < 8; k++) {
   	    	for(int m = 0; m < 8; m++) {
   	    		printf("%d\t", matrix_c[k][m]);
   	    	}
   	    	printf("\n");
   	    }
   		free_buffer(&matrix_a);
   		free_buffer(&matrix_c);
   } else {
   		MPI_Bcast(&(matrix_b[0][0]), 64, MPI_INT, 0, MPI_COMM_WORLD);
   		int **output_rows; //individual results of matrix row calculations
   		
   		MPI_Recv(&(rows[0][0]), 16, MPI_INT, 0, MASTER,
   					 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   		init_buffer(&output_rows, 2, 8);
   		
   		for(int i = 0; i < 2; i++) {
   			int *current_row = get_row(rows, i);
   			
   			for(int j = 0; j < 8; j++) {
   				int *current_column = get_column(matrix_b, j);
   				output_rows[i][j] = mul_add(current_row, current_column);
   			}
   		}
   		MPI_Send(&(output_rows[0][0]), 16, MPI_INT, 0, SLAVE,
   				 MPI_COMM_WORLD);
   		free_buffer(&output_rows);
   	
   }
   free_buffer(&matrix_b);
   free_buffer(&rows);
   MPI_Finalize();
}

int *get_row(int **matrix, int row) {
	return matrix[row];
}

int *get_column(int **matrix, int col) {
	int *column = (int*)malloc(sizeof(int) * 8);
	
	for(int i = 0; i < 8; i++) {
		column[i] = matrix[i][col];
	}
	return column;
}

int mul_add(int *row, int *col) {
	int result = 0;
	
	for(int i = 0; i < 8; i++) {
		result += row[i] * col[i];
	}
	return result;
}

int init_buffer(int ***buf, int n, int m) {
    int *mem = (int*)malloc(n * m * sizeof(int));
    
    if(!mem) { return -1; }
    
    (*buf) = (int**)malloc( n * sizeof(int*));
    
    if(!(*buf)) {
        free(mem);
        return -1;
    }
    
    for(int i = 0; i < n; i++) {
        (*buf)[i] = &(mem[i * m]);
    }
    return 0;
}

int free_buffer(int ***buf) {
    free(&((*buf)[0][0]));
    free(*buf);
    return 0;   
}
