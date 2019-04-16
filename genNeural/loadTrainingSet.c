#include "loadTrainingSet.h"

#define BUFFER_SIZE 256

int parseSet(FILE *fp, double ***inputSet, int *numInputs,
	double ***outputSet, int *numOutputs)
{
	int numSets;

	char buffer[BUFFER_SIZE];
	char junk;

	int fileLine = 0;
	int scan;

	if (fgets(buffer, BUFFER_SIZE, fp) != NULL)
	{
		fileLine++;
		scan = sscanf(buffer, "%d %d %d", &numSets, numInputs, numOutputs);
		if (scan == EOF || scan == 0)
		{
			fprintf(stderr, "Unable to find set details at start of file!\n");
			fprintf(stderr, "Expecting number of sets, inputs, and outputs.\n");
			return 0;
		}
	}
	else
	{
		fprintf(stderr, "Unable to read file!\n");
		return 0;
	}
	if (numSets <= 0 || *numInputs <= 0 || *numOutputs <= 0)
	{
		fprintf(stderr, "Set details must all be positive intergers!\n");
		return 0;
	}

	//allocate sets
	*inputSet = (double**)malloc(numSets * sizeof(double*));
	*outputSet = (double**)malloc(numSets * sizeof(double*));
	for (int i = 0; i < numSets; i++)
	{
		(*inputSet)[i] = (double*)calloc(*numInputs, sizeof(double));
		(*outputSet)[i] = (double*)calloc(*numOutputs, sizeof(double));
	}

	//**/fprintf(stderr, "buffer: %s\n", buffer);
	//**/fprintf(stderr, "set(%d) input(%d) output(%d)\n", numSets, *numInputs, *numOutputs);

	//load values into sets
	double x;
	int count = 0;
	int placement;
	int set;
	int numData = numSets * (*numInputs + *numOutputs);
	while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
	{
		fileLine++;

		scan = sscanf(buffer, "%lf %c", &x, &junk);
		if (scan == EOF) break;
		if (scan == 1)
		{
			placement = count % (*numInputs + *numOutputs);
			set = count / (*numInputs + *numOutputs);
			++count;

			if (count > numData)
				break;

			if (placement < *numInputs)
			{
				//**/fprintf(stderr, "(*inputSet)[%d][%d] = %.3f\n", set, placement, x);
				(*inputSet)[set][placement] = x;
			}
			else
			{
				//**/fprintf(stderr, "(*outputSet)[%d][%d] = %.3f\n", set, placement, x);
				(*outputSet)[set][placement - *numInputs] = x;
			}

		}
		else if (scan > 1)
		{
			fprintf(stderr, "Improper formatting of set at line %d\n", fileLine);
			return 0;
		}
	}

	if (count != numData)
	{
		fprintf(stderr, "Mismatch with count of expected set data! Counted up to %d entries, but expected %d\n", count, numData);
		return 0;
	}

	return numSets;
}

void printTrainingSet(int setSize, double **inputSet, int numInputs,
	double **outputSet, int numOutputs)
{
	for (int set = 0; set < setSize; set++)
	{
		fprintf(stderr, "[");
		for (int i = 0; i < numInputs; i++)
		{
			fprintf(stderr, "%.3f ", inputSet[set][i]);
		}
		fprintf(stderr, "]");

		fprintf(stderr, " | [");
		for (int i = 0; i < numOutputs; i++)
		{
			fprintf(stderr, "%.3f ", outputSet[set][i]);
		}
		fprintf(stderr, "]");
		fprintf(stderr, "\n");
	}
}