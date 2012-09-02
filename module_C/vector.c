#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <malloc.h>
#include <stdint.h>
#include "conf.h"
#include "vector.h"
#include "stopwatch.h"

char *W;
char *Wt;
stopwatch * sw;

int isInVector(int tab[],int tab_length, int value){
	int i =0;
	for (i=0;i<tab_length;i++){
		if (tab[i]==value){ return TRUE;}
	}
	return FALSE;
}

void generate_vector(int size, double probability, int variance, vector *vector){
	//Compute the mean number of active node (the +0.5 is needed to have a correct round function to an integer)
	int mean_active = (int)((size*probability/100.0)+0.5);
	//printf("should be active : %d\n",mean_active);
	//Compute the total active node for the vector (will be between: [mean-variance;mean+variance]
    /* For the record:
     *  there is one problem in this line: RAND_MAX is 2147483647, which is
     *  0x7fffffff. Adding 1.0 gives a negative value on 32 bits processors,
     *  probably because of the sign bit :) 
	int active_neuron = (int) (mean_active - variance + (rand()*(variance*2+1.0) / ( RAND_MAX + 1.0 ) ));
    */
	int active_neuron = (int) (mean_active - variance + (rand()*(variance*2.0) / RAND_MAX ));

	//active number of node can't be greater than the vector length
	if (active_neuron>size) { active_neuron=size;}

	//printf("active : %d\n",active_neuron);

	int *index=malloc(sizeof(int)*active_neuron);

	int i,j,temp=0;
	int swap=FALSE;
	int miss=0;
	//init sparse vector
	for (i=0;i<active_neuron;i++){
		index[i]=-1;
	}
	//Fill vector with indexes of active neuron
	for (i=0;i<active_neuron;i++){
		//to prevent looping forever (in case of large probabilities), break after having to recompute an index 3times the size
		//of the vector array (without finding a valid one)
		if (miss >= 3*size) { printf("Probability to activate neuron is too high, you might want to consider a faster function to generate your vectors\n"); break;}
        /* there was a buffer overflow here: rand() * size was performed before
         * the division */
		int indextemp = (int) (size * (rand() / (double) RAND_MAX ));
		//Can't activate twice the same neuron
		if (isInVector(index,active_neuron,indextemp)==TRUE){
			i=i-1;
			miss++;
			continue;
		}

		//place it at the correct place and move other if neccessary
		for (j=0;j<=i;j++){

			if (swap==TRUE){
				//we need to move all element 1 case to the right
				if (index[j]==-1){
					index[j]=indextemp;
					break;
				}
				else{
					temp=index[j];
					index[j]=indextemp;
					indextemp=temp;
				}
			}
			//we are on a unassigned square
			if (index[j]==-1 && swap==FALSE){
				index[j]=indextemp;
				break;
			}
			//the index is smaller than the current square, so it needs to be put there
			if (swap==FALSE && indextemp<index[j]){
				swap=TRUE;
				temp=index[j];
				index[j]=indextemp;
				indextemp=temp;
			}

		}
		swap=FALSE;

		//index[i]=indextemp;
		miss=0;
	}
	vector->size=size;
	vector->active_neuron=active_neuron;
	vector->index=index;
}

void generate_lexicon(int numberOfSymbols,int n,double probability, int variance,TCSymbol **symbollist){
	int i,j,k=0;
	//TCSymbol *TCList = malloc(sizeof(TCSymbol)*numberOfSymbols);

	for (i=0;i<numberOfSymbols;i++){
		//create a vector x and y
		TCSymbol *symbol = malloc(sizeof(TCSymbol));
		vector *x = malloc(sizeof(vector));
		vector *y = malloc(sizeof(vector));
		symbol->x=x;
		symbol->y=y;
		generate_vector(n,probability,variance,symbol->x);
		generate_vector(n,probability,variance,symbol->y);
		symbollist[i]=symbol;
		for (j=0;j<symbol->x->active_neuron;j++){
			for (k=0;k<symbol->y->active_neuron;k++){
				int index = ((symbol->x->index[j])*N)+(symbol->y->index[k]);
				SETBIT(W,index);
				int indext = ((symbol->y->index[k])*N)+(symbol->x->index[j]);
				SETBIT(Wt,indext);
				//printf("bit set at [%d;%d](%d) : %d",j,k,index,ISBITSET(W,index));
			}
		}

	}
}

void compute_excitation(vector *u,int transpose,int range_active, vector *result){
	int *tempvector = malloc(sizeof(int)*N);
	int i,j=0;
	int index=0;
	int newindex=0;
	for (i=0;i<N;i++){
		tempvector[i]=0;
	}
	//compute excitation and store it in a temporary vector
	for (i=0;i<u->active_neuron;i++){
		if (transpose==FALSE){
			index = N*u->index[i];
			newindex = index;
			for (j=0;j<N;j=j+8){
				newindex = index+j;
				char isbitsetA = W[newindex>>3];
				if (isbitsetA & 1) tempvector[j]++;
        			if (isbitsetA & 2) tempvector[j+1]++;
        			if (isbitsetA & 4) tempvector[j+2]++;
        			if (isbitsetA & 8) tempvector[j+3]++;
       				if (isbitsetA & 16) tempvector[j+4]++;
       				if (isbitsetA & 32) tempvector[j+5]++;
       				if (isbitsetA & 64) tempvector[j+6]++;
       				if (isbitsetA & 128) tempvector[j+7]++;
			}
		}
		else{
			index = N*u->index[i];
			newindex = index;
			for (j=0;j<N;j=j+8){
				newindex = index+j;
				char isbitsetA = Wt[newindex>>3];
				if (isbitsetA & 1) tempvector[j]++;
        			if (isbitsetA & 2) tempvector[j+1]++;
        			if (isbitsetA & 4) tempvector[j+2]++;
        			if (isbitsetA & 8) tempvector[j+3]++;
       				if (isbitsetA & 16) tempvector[j+4]++;
       				if (isbitsetA & 32) tempvector[j+5]++;
       				if (isbitsetA & 64) tempvector[j+6]++;
       				if (isbitsetA & 128) tempvector[j+7]++;
			}
		}
	}
	//make active the highest neurons in the tempvector

	int count=0;
	//first get maximum of "input excitation"
	int max = tempvector[0];
	for (i=0;i<N;i++){
		if (tempvector[i]>max) max=tempvector[i];
	}
	//then active only the neurons in the range of that max
	//Note that the second condition && I[i]!=0 is there to prevent to activate neurons who didn't receive any
	//input excitation
	for (i=0;i<N;i++){
		if (max-tempvector[i]<= range_active && tempvector[i]!=0) {
			tempvector[i]=1;
			count++;
		}
		else tempvector[i]=0;
	}

	//create a vector from tempvector

	result->size=N;
	result->active_neuron=count;
	int *retindex = malloc(sizeof(int)*count);
	j = 0;
	for (i=0;i<N;i++){
		if (tempvector[i]== 1){
			retindex[j]=i;
			j++;
		}
	}
	result->index=retindex;
	//free tempvector and return vector
	free(tempvector);
}

void execute_step(vector *v, int range_active, vector *result){
	vector *temp = malloc(sizeof(vector));
	compute_excitation(v,FALSE,range_active,temp);
	compute_excitation(temp,TRUE,range_active,result);
}

/* This flag is essential to avoid name conflicts when linking other object
 * files with this one */
#ifdef COMPILE_MAIN_VECTOR_C
int main(int argc,char *argv[]){
	sw = sw_init_clocktype(CLOCK_PROCESS_CPUTIME_ID);
	printf("Creating module...\n");
	srand(time(NULL));
	//vector *a = malloc(sizeof(vector));
	//generate_vector(10,100,1,a);

	TCSymbol **TCList = malloc(sizeof(TCSymbol)*L);
	W = malloc(sizeof(char)*WSize);
	Wt = malloc(sizeof(char)*WSize);

	generate_lexicon(L,N,P,V,TCList);

	printf("Done!\n");
	printf("Testing confabulation algorithm...\n");
	vector *t = malloc(sizeof(vector));
	int i = 0;
	for (i=0;i<L;i++){	
	sw_start(sw);
	execute_step(TCList[i]->x,0,t);
	sw_stop(sw);
	}
	printf("Done!\n");
	//struct mallinfo minfo;
	//minfo = mallinfo();
	//printf("memory usage : %d %d %d %d\n",minfo.uordblks,minfo.usmblks,minfo.hblkhd, minfo.arena);

	printf("Total cpu time usage for the algorithm:\n");
    	sw_print(sw);
   	sw_free(sw); /* don't forget :) */
	return 0;
}
#endif /* COMPILE_MAIN_VECTOR_C */
