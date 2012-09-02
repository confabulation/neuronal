typedef struct vectorstruct {
	int size;
	int active_neuron;
	int *index;
} vector;

typedef struct thalamocorticalsymbol {
	vector *x;
	vector *y;
} TCSymbol;

typedef struct matrixtriplet{
	int x;
	int y;
	int value;
} triplet;

typedef struct matrix{
	int element;
	triplet **normal;
	triplet **transposee;
} matrix;
