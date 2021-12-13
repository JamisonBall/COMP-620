#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

//function headers
void par_quicksort(int*, int, int);
int partition(int*, int, int);

int main(int argc, char **argv) {
	int *data = (int*)malloc(sizeof(int) * 25); //create array to hold data
	time_t seed; //for generating random numbers
	double omp_sort_start;
	double omp_sort_finish;
	
	int i;
	int j;
	int k;
	
	srand(time(&seed));
	#pragma omp parallel for shared(data, i) num_threads(5)
	for(i = 0; i < 25; i++) {
		data[i] = rand() % 25;
	}
	
	printf("Before sort:\n");
	#pragma omp parallel for shared(data, j) num_threads(5)
	for(j = 0; j < 25; j++) {
		printf("%d ", data[j]);
	}
	printf("\n");
	omp_sort_start = omp_get_wtime();
	par_quicksort(data, 0, 24);
	omp_sort_finish = omp_get_wtime();
	printf("After sort:\n");
	
	for(k = 0; k < 25; k++) {
		printf("%d ", data[k]);
	}
	printf("\n");
	
	printf("Parallel sort finished in %f seconds.\n",
			omp_sort_finish - omp_sort_start);
	free(data);
	return 0;
}

void par_quicksort(int *data, int p, int r) {
	if(p < r) {
		int q = partition(data, p, r);
		
		#pragma omp parallel shared(data, p, q, r) num_threads(2)
		{
		//I chose to use 2 threads here to limit idle threads
			#pragma omp sections
			{
				#pragma omp section
				{
					par_quicksort(data, p, q - 1);
				}
				#pragma omp section
				{
					par_quicksort(data, q + 1, r);
				}
			}
		}
		
	}
}


int partition(int *data, int p, int r) {
	int x = data[r];
	int i = p - 1;
	int j;
	int temp;
	
	for(j = p; j < r; j++) {
		if(data[j] <= x) {
			i++;
			temp = data[i];
			data[i] = data[j];
			data[j] = temp;
			
		}
	}
	
	temp = data[i + 1];
	data[i + 1] = data[r];
	data[r] = temp;
	return i + 1;
}
