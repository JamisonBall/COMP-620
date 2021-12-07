void par_odd_even_sort(int*, int);

int main(int argc, char **argv) {
	int *data = (int*)malloc(sizeof(int) * 25); //create array to hold data
	time_t seed; //for generating random numbers
	
	//iterators
	int i;
	int j;
	int k;
	
	srand(time(&seed));
	
	//use threads to quickly generate numbers to fill array
	#pragma omp parallel for default(none), shared(data,i) num_threads(5)
	for(i = 0; i < 25; i++) {
		data[i] = rand() % 25;
	}
	
	printf("Before sort:\n");
	
	#pragma omp parallel for default(none), shared(data,j) num_threads(5)
	for(j = 0; j < 25; j++) {
		printf("%d ", data[j]);
	}
	printf("\n");
	
	par_odd_even_sort(data, 25);
	printf("After sort:\n");
	
	for(k = 0; k < 25; k++) {
		printf("%d ", data[k]);
	}
	printf("\n");
	return 0;
}

void par_odd_even_sort(int *data, int size) {
	int i;
	int even_odd;
	int temp;
	
	for(even_odd = 0; even_odd < size; even_odd++) {
		if(even_odd % 2 == 0) {
			#pragma omp parallel for default(none) shared(data,size) private(i,temp) num_threads(5)
			for(i = 1; i < size - 1; i += 2) {
				if(data[i- 1] > data[i]) {
					temp = data[i];
					data[i] = data[i - 1];
					data[i - 1] = temp;
				}
			}
		} else {
			#pragma omp parallel for default(none) shared(data,size) private(i,temp) num_threads(5)
			for(i = 1; i < size; i += 2) {
				if(data[i] > data[i + 1]) {
					temp = data[i];
					data[i] = data[i + 1];
					data[i + 1] = temp;
				}
			}
		}
	}
}
