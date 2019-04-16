#include <errno.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include<time.h> //srand

#include "netGen.h"
#include "loadWgtGraph.h"
#include "loadTrainingSet.h"

#define DEGREE (0.25f)

#define SUCCESS  0
#define FAILURE  1
#define FILEFAIL 2

char* programName = NULL;
int exitStatus = SUCCESS;

void printUsage()
{
	printf("usage: %s [-L] graph.file traingSet.file\n", programName);
	printf("FLAGS: \n");
	printf(" -L    Use provided weights, instead of random\n");
}

void printError(const char* object, const char* message)
{
	fflush(NULL);
	fprintf(stderr, "%s: %s: %s\n", programName, object, message);
	fflush(NULL);
	exitStatus = FAILURE;
}

FILE* openFile(const char* filename)
{
	FILE* fp;
	if (strcmp(filename, "-") == 0)
		fp = stdin;
	else
		fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printError(filename, strerror(errno));
		exitStatus = FILEFAIL;
	}
	return fp;
}

void closeFile(FILE* fp)
{
	int status;
	if (fp != NULL)
	{
		status = fclose(fp);
	}
	if (status == EOF)
	{
		printError("Unable to close file", strerror(errno));
		exitStatus = FILEFAIL;
	}
}

//My makeshift getopt
int optind = 1;
int optopt;

int getopt(int argc, char** argv, char* str)
{
	if (argc <= optind)
		return EOF;

	if (argv[optind][0] != '-' || strcmp(argv[optind], "-") == 0)
		return EOF;

	optopt = (int)argv[optind++][1];

	for (int i = 0; str[i] != '\0'; i++)
		if (optopt == str[i])
			return optopt;

	return (int)'?';
}

void scanOptions(int argc, char** argv,
	int* flag)
{
	for (;;)
	{
		int option = getopt(argc, argv, "L");
		if (option == EOF) break;
		switch (option)
		{
			char optoptString[16];
		case 'L':
			*flag = (int)'L';
			break;
		case '?':
			sprintf(optoptString, "-%c", optopt);
			printError(optoptString, "invalid option");
			printUsage();
			exit(exitStatus);
			break;
		}
	}
}

double randDouble()
{
	int mod = 12345;
	int random = rand() % mod;
	return (double)random / (double)mod;
}

#define CONSTANT (1.f)

int main(int argc, char *argv[])
{
	srand(2); //seed time(0)

	programName = basename(argv[0]);
	int flag = 0;

	if (argc == 1)
	{
		printUsage();
		return SUCCESS;
	}

	scanOptions(argc, argv, &flag);

	FILE *fp = NULL;
	char* filename = NULL;

	//load graph
	if (argc <= optind)
	{
		printError("No graph file specified.", "For stdin, use -");
		printUsage();
		return FAILURE;
	}
	filename = argv[optind];

	fp = openFile(filename);
	if (fp == NULL)
		return FILEFAIL;

	Node **nodeList;
	int numNodes;
	if (flag == (int)'L')
		nodeList = parseEdges(fp, &numNodes, NULL);
	else
		nodeList = parseEdges(fp, &numNodes, randDouble);

	closeFile(fp);

	//**/fprintf(stderr, "main %d %s %x\n", nodeList[3]->id, nodeList[3]->func->name, (int)nodeList[3]->func->norm);

	///**/printNodeList(nodeList, numNodes);

	//load training set
	if (argc <= ++optind)
	{
		printError("No training set file specified.", "For stdin, use -");
		printUsage();
		return FAILURE;
	}
	filename = argv[optind];

	fp = openFile(filename);
	if (fp == NULL)
		return FILEFAIL;

	int setSize = 0;
	int numInputs = 0;
	int numOutputs = 0;
	double **inputSet = NULL;
	double **outputSet = NULL;

	setSize = parseSet(fp, &inputSet, &numInputs, &outputSet, &numOutputs);
	if (setSize == 0)
		return FAILURE;

	///**/printTrainingSet(setSize, inputSet, numInputs, outputSet, numOutputs);

	double minError = 0.001f;
	int cutOff = 500000;

	double retErr = -1.f;
	retErr = train(nodeList, numNodes, inputSet, numInputs, outputSet, numOutputs, setSize, rand, minError, cutOff, DEGREE);
	fprintf(stderr, "Avg. error = %.3f < %.3f\n", retErr, minError);

	for (int set = 0; set < setSize; set++)
	{
		//clear network
		for (int i = 0; i < numNodes; i++)
		{
			resetNode(nodeList[i]);
		}

		//load input to nodeList
		for (int i = 0; i < numInputs; i++)
		{
			nodeList[i]->out = inputSet[set][i];
			//nodeList[i]->status = ON;
		}

		//run network forward
		for (int i = 0; i < numNodes; i++)
		{
			if (nodeList[i]->status == OFF)
				forwards(nodeList[i]);
		}

		//calulate error for network
		double err = 0;
		for (int i = numNodes - numOutputs; i < numNodes; i++)
		{
			int iout = i - (numNodes - numOutputs);
			double currErr = errorNorm(nodeList[i]->out, outputSet[set][iout]);
			///**/printf("\n out,set = %.3f,%.3f\n", nodeList[i]->out, outputSet[set][iout]);
			///**/printf("\n err = %.3f, id(%d)\n", currErr, nodeList[i]->id);
			err += currErr;
		}

		//print results
		fprintf(stderr, "[");
		for (int i = 0; i < numInputs; i++)
		{
			fprintf(stderr, "%.3f ", nodeList[i]->out);
		}
		fprintf(stderr, "]");

		fprintf(stderr, " | [");
		for (int i = numNodes - numOutputs; i < numNodes; i++)
		{
			fprintf(stderr, "%.3f ", nodeList[i]->out);
		}
		fprintf(stderr, "]");

		fprintf(stderr, " - [");
		for (int i = 0; i < numOutputs; i++)
		{
			fprintf(stderr, "%.3f ", outputSet[set][i]);
		}
		fprintf(stderr, "]");
		fprintf(stderr, " (%.3f)\n", err);
	}

	printNodeList(nodeList, numNodes);

	return 0;
}