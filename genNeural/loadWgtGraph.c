/* Levi J. Hall
 * ljhall@ucsc.edu
 */

#include "loadWgtGraph.h"

#define BUFFER_SIZE 256

Node **parseEdges(FILE *fp, int *length, double(*randFunc)())
{
	Node **nodeList;
	int countV;
	int from, to;
	double weight;

	char buffer[BUFFER_SIZE];
	char junk;

	int fileLine = 0;
	int scan;

	if (fgets(buffer, BUFFER_SIZE, fp) != NULL)
	{
		fileLine++;
		scan = sscanf(buffer, "%d", &countV);
		if (scan == EOF || scan == 0)
		{
			fprintf(stderr, "Unable to find number of vertices at start of file!\n");
			return NULL;
		}
	}
	else
	{
		fprintf(stderr, "Unable to read file!\n");
		return NULL;
	}

	if (countV <= 0)
	{
		fprintf(stderr, "A positive integer was expected at line %d: %d\n", fileLine, countV);
		return NULL;
	}

	//make network graph
	nodeList = (Node**)malloc(countV * sizeof(Node*));
	*length = countV;

	NetFunction * funcList = initFuncList();

	int ifunc;
	for (int i = 0; i < countV; i++)
	{
		ifunc = getFuncByName("TANH");
		nodeList[i] = makeNode(&funcList[ifunc], i + 1);
	}

	//**/fprintf(stderr, "func 0: %s\n", funcList[0].name);
	//**/fprintf(stderr, "func 1: %s\n", funcList[1].name);
	//**/fprintf(stderr, "func 2: %s\n", funcList[2].name);
	//**/fprintf(stderr, "func 3: %s\n", funcList[3].name);

	char funcName[4];
	while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
	{
		fileLine++;
		scan = sscanf(buffer, "f %d %4s %c", &from, funcName, &junk);
		if (scan == EOF) break;
		if (scan != 0)
		{
			if (scan == 2)
			{
				if (from < 1 || from > countV)
				{
					fprintf(stderr, "FROM vertex out of bounds at line %d\n", fileLine);
					return NULL;
				}

				ifunc = getFuncByName(funcName);
				if (ifunc == -1)
				{
					fprintf(stderr, "Unreginized function %s at line %d\n", funcName, fileLine);
					return NULL;
				}

				nodeList[from - 1]->func = &funcList[ifunc];
			}
			else
			{
				fprintf(stderr, "Improper formatting of edges at line %d\n", fileLine);
				return NULL;
			}
			continue;
		}

		scan = sscanf(buffer, "%d %d %lf %c", &from, &to, &weight, &junk);
		if (scan == EOF) break;
		if (scan == 2)
			weight = 0.0f;
		else if (scan != 3)
		{
			fprintf(stderr, "Improper formatting of edges at line %d\n", fileLine);
			return NULL;
		}

		if (from < 1 || from > countV)
		{
			fprintf(stderr, "FROM vertex out of bounds at line %d\n", fileLine);
			return NULL;
		}
		if (to < 1 || to > countV)
		{
			fprintf(stderr, "TO vertex out of bounds at line %d\n", fileLine);
			return NULL;
		}

		if (randFunc == NULL)
			nodePush(nodeList[to - 1], nodeList[from - 1], weight);
		else
			nodePush(nodeList[to - 1], nodeList[from - 1], (*randFunc)());
	}


	//free unreferenced functions
	//free funcList

	//**/fprintf(stderr, "load %d %s %x\n", nodeList[3]->id, nodeList[3]->func->name, (int)nodeList[3]->func->norm);

	return nodeList;
}

void printNodeList(Node **nodeList, int numNodes)
{
	printf("%d\n", numNodes);
	for (int i = 0; i < numNodes; i++)
	{
		printf("f %d %s\n", nodeList[i]->id, nodeList[i]->func->name);
	}
	for (int i = 0; i < numNodes; i++)
	{
		for (int j = 0; j < nodeList[i]->numInputs; j++)
		{
			printf("%d %d %f\n", nodeList[i]->inputs[j]->id, nodeList[i]->id, nodeList[i]->weights[j]);
		}
	}
}
