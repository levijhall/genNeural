#include <stdio.h>

#include "node.h"

#ifndef C101netGen
#define C101netGen

void applyNorm(Node *node);

void resetNode(Node *node);

void forwards(Node *node);

double errorNorm(double hypoth, double expected);

double errorDerv(Node *node, double expected);

void recurDelta(Node *node);

void trainWeights(Node *node, double degree);

//precondition: nodeList[] has at least numOutputs elements initialized and allocated
void backwards(Node *nodeList[], int length, double outputSet[], int numOutputs, double degree);

double train(Node *nodeList[], int numNodes, double *inputSet[], int numInputs,
	double *outputSet[], int numOutputs, int setSize, int(*randFunc)(), double minError,
	int cutOff, double degree);

#endif //C101netGen