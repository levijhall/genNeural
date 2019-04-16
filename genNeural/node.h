#include <stdlib.h>
#include <stdio.h>

#include "netFunction.h"

#ifndef C101Node
#define C101Node

#define InitCap 4

typedef enum state
{
	OFF,
	SIGNAL,
	CYCLE,
	ON,
	DELTA,   //strictly an edge relation
	TRAINED, //strictly an edge relation
	LENGTH
} State;

struct node;
typedef struct node {
	int id;
	State status;

	int numInputs;
	int capacity;
	struct node **inputs; //nodes with edges leading into this node
	State *relations; //record which value was used with this edge (prev (CYCLE) or curr (ON))
	double *weights; //weights for each edge from inputs

	//Normalization function and derivative
	NetFunction *func;

	//stored values
	double curr;
	double prev;
	double out;

	//value used to alter weights in training
	double delta;
} Node;

Node *makeNode(NetFunction *func, int id);

void nodePush(Node *node, Node *input, double weight);

void printState(State x);

#endif //C101Node