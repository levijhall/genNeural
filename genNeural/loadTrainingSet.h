#include <stdio.h>
#include <stdlib.h>

//form the input and output sets
//#sets #input #output, say 3 and 2
//2 3 2
//i1
//i2
//i3
//o1
//o2
//i1
//i2
//i3
//o1
//o2
int parseSet(FILE *fp, double ***inputSet, int *numInputs,
	double ***outputSet, int *numOutputs);

void printTrainingSet(int setSize, double **inputSet, int numInputs,
	double **outputSet, int numOutputs);