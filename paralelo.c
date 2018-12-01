#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>



typedef struct bucket{
	int size;  //Tamanho do bucket
	int start; //Inicio de cada bucket
	int index; //Indice para inserir no bucket;
}*Bucket;

int cache[30000000];


int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

static int cmpdouble (const void * a, const void * b)
{
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;  
}

void clear_cache() {
    for(unsigned i = 0; i < 30000000; i++) {
        cache[i] = i;
    }
}


int* bucket_sort(int* array, int nelem, int nbuckets, int limit){

	int *new = calloc(nelem,sizeof(int)); //Criação do array a ser ordenado
	Bucket buckets = calloc(nbuckets,sizeof(struct bucket)); //Criação dos buckets

	int i,b;
	//Contagem de elementos para bucket
	for(i = 0; i < nelem; i++){
		b = (array[i] * nbuckets) / limit;         //Calcula o bucket a ser ulilizado
		if (!(buckets[b].size)) buckets[b].size = 1; //Bucket vazio
		else buckets[b].size++;						   //Incrementa o size do bucket a ser utilizado
	}

	//Atribuir o inicio de cada bucket no array que vai ser ordenado
	buckets[0].start = 0; //Primeiro bucket começa no indice 0 do array
	buckets[0].index = 0; 

	for(i = 1; i < nbuckets; i++){
		buckets[i].start = buckets[i-1].start + buckets[i-1].size; //Com o tamanho de cada bucket calculamos o inicio do proximo
		buckets[i].index = buckets[i].start;
	}


	//Colocar no array final de acordo com o bucket
	//Ordenação de cada bucketse	
	for (i = 0; i < nelem ; i++){
		b = array[i] * nbuckets / limit; //Calcula o bucket a ser ulilizado
		new[buckets[b].index] = array[i];  //Insere no bucket
		buckets[b].index++;	
	}
	//Ordenação de cada bucket
	#pragma omp parallel for
	for(i = 0; i < nbuckets; i++)
		qsort(&new[buckets[i].start],buckets[i].size, sizeof(int), cmpfunc);

	return new;
}

int main (int argc, char const ** argv){
	int array_size = atoi(argv[1]);
	int *unsorted = malloc(array_size*sizeof(int));
	double time[20],t;

	int n_iter,i,ord=1, *sorted;
	for (n_iter = 0; n_iter < 20 ; n_iter++){
		

		for(i = 0; i< array_size; i++){
			unsorted[i] = (int) random() % 500;
		}
		clear_cache();
		t = omp_get_wtime();
		sorted = bucket_sort(unsorted,array_size,atoi(argv[2]),501);
		time[n_iter] = omp_get_wtime() - t;
		for (i=0; i < array_size-1; i++) {
            if (sorted[i] > sorted[i+1]) 
                ord = 0;
        }


	}
	qsort(time, 20, sizeof(&time[0]), cmpdouble);
    if (ord == 1) printf("%f ",(time[10]+time[11])/2.0); 
    else printf("%f ",-1.0);
}