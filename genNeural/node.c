#include "node.h"

Node *makeNode(NetFunction *netFunc, int id)
{
	Node *node = (Node*)malloc(sizeof(Node));
	node->id = id;
	node->status = OFF;

	node->numInputs = 0;
	node->capacity = InitCap;
	node->inputs = (Node**)calloc(InitCap, sizeof(Node*));
	node->relations = (State*)calloc(InitCap, sizeof(State));
	node->weights = (double*)calloc(InitCap, sizeof(double));

	node->func = netFunc;

	node->curr = 0.f;
	node->prev = 0.f;
	node->out = 0.f;
	node->delta = 0.f;

	return node;
}

void nodePush(Node *node, Node *input, double weight)
{
	node->numInputs++;
	if (node->numInputs > node->capacity)
	{
		node->capacity *= 2;
		node->inputs = (Node**)realloc(node->inputs, node->capacity * sizeof(Node*));
		node->relations = (State*)realloc(node->relations, node->capacity * sizeof(State));
		node->weights = (double*)realloc(node->weights, node->capacity * sizeof(double));
	}
	node->inputs[node->numInputs - 1] = input;
	node->relations[node->numInputs - 1] = OFF;
	node->weights[node->numInputs - 1] = weight;
}

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void printState(State x)
{
	switch (x)
	{
	case OFF:
		//printf(WHT);
		printf("OFF");
		break;
	case SIGNAL:
		//printf(RED);
		printf("SIGNAL");
		break;
	case CYCLE:
		//printf(RED);
		printf("CYCLE");
		break;
	case ON:
		//printf(RED);
		printf("ON");
		break;
	case DELTA:
		//printf(RED);
		printf("DELTA");
		break;
	case TRAINED:
		//printf(RED);
		printf("TRAINED");
		break;
	default:
		printf("Inv");
	}
	//printf(RESET);
}