#include "netGen.h"

#define DEBUG 0

void applyNorm(Node *node)
{
	//**/fprintf(stderr, "netG %d\n", node->id);
	//**/fprintf(stderr, "%s %x\n", node->func->name, (int)node->func->norm);
	node->out = (*node->func->norm)(node->curr);
}

void resetNode(Node *node)
{
	node->status = OFF;
	node->prev = node->out;
	node->curr = 0;
	node->out = 0;
	node->delta = 0;
	for (int i = 0; i < node->numInputs; i++)
		node->relations[i] = OFF;
}

void printNode(Node *node, int flag)
{
	printf("id(%d) ", node->id);
	printf("status(");
	printState(node->status);
	printf(") ");
	printf("curr(%.3f) prev(%.3f) out(%.3f) delta(%.3f)", node->curr, node->prev, node->out, node->delta);
	if (flag == 1)
	{
		for (int i = 0; i < node->numInputs; i++)
		{
			printf("\n\t[id(%d) ", node->inputs[i]->id);
			printf("rel(");
			printState(node->relations[i]);
			printf(") wgt(%.3f)]", node->weights[i]);
		}
	}
	printf("\n");
}

void forwards(Node *node)
{
	if (node->status == CYCLE || node->status == ON)
		return;


	int isCYCLE = 0;
	for (int i = 0; i < node->numInputs; i++)
	{
		Node *input = node->inputs[i];
		double edgeWeight = node->weights[i];

		if (input->status == OFF)
		{
			input->status = SIGNAL;
			forwards(input);
		}
		if (node->status != CYCLE && node->status != ON)
		{
			if (input->status == ON)
			{
				node->curr += input->out * edgeWeight;
				node->relations[i] = ON;
			}
			if (node->status == OFF && input->status == CYCLE)
			{
				node->curr += input->out * edgeWeight;
				node->relations[i] = ON;
			}
			if (node->status == SIGNAL && (input->status == SIGNAL || input->status == CYCLE))
			{
				node->curr += input->prev * edgeWeight;
				node->relations[i] = CYCLE;
				isCYCLE = 1;
			}
			if (node->status == OFF && input->status == SIGNAL)
				fprintf(stderr, "Unresolved scenario: OFF node found SIGNAL input\n");
		}
	}
	if (node->status != CYCLE && node->status != ON)
	{
		if (isCYCLE)
			node->status = CYCLE;
		else
			node->status = ON;

		if (node->numInputs > 0)
			applyNorm(node);

		if (DEBUG) printNode(node, 1);
	}
}

//inputs (the first n nodes) (this must include the constants) (must not have edges leading to inputs)
//outputs (the last m nodes)

double errorNorm(double hypoth, double expected) //Mean-square error
{
	double err;
	err = (hypoth - expected) * (hypoth - expected);
	return err;
}

//Only called on outputs, so CYCLES should never happen
double errorDerv(Node *node, double expected)
{
	double derv;
	derv = 2.f * (node->out - expected) * (*node->func->derv)(node->curr);
	return derv;
}

void recurDelta(Node *node)
{
	Node *input = NULL;
	for (int i = 0; i < node->numInputs; i++)
	{
		input = node->inputs[i];

		if (node->relations[i] == DELTA)
			continue;

		if (node->inputs[i]->numInputs < 0)
			continue;

		if (node->relations[i] == ON)
		{
			input->delta += node->delta * node->weights[i] * (*input->func->derv)(input->curr);
			node->relations[i] = DELTA;

			if (DEBUG) printf("ON\n");
			if (DEBUG) printNode(node, 1);
			recurDelta(input);
			continue;
		}
		if (node->relations[i] == CYCLE)
		{
			input->delta += node->delta * node->weights[i] * (*input->func->derv)(input->prev);
			node->relations[i] = DELTA;

			if (DEBUG) printf("CYCLE\n");
			if (DEBUG) printNode(node, 1);
			recurDelta(input);
			continue;
		}
		else
			fprintf(stderr, "Unexpected edge relation in training\n");
	}
}

void trainWeights(Node *node, double degree)
{
	for (int i = 0; i < node->numInputs; i++)
	{
		Node *input = node->inputs[i];

		if (node->relations[i] == TRAINED)
			continue;

		if (node->relations[i] == DELTA)
		{
			node->weights[i] -= degree * node->delta * input->out;
			node->relations[i] = TRAINED;

			if (DEBUG) printNode(node, 1);
			trainWeights(input, degree);
		}
		else
			fprintf(stderr, "Unexpected edge relation in weight training\n");
	}
}

//precondition: nodeList[] has at least numOutputs elements initialized and allocated
void backwards(Node *nodeList[], int length, double outputSet[], int numOutputs, double degree)
{
	//Calculate deltas
	Node *node = NULL;
	Node *input = NULL;

	if (DEBUG) printf("backwards\n");
	for (int i = 0; i < numOutputs; i++)
	{
		if (DEBUG) printf("outputSet[%d] = %.3f\n", i, outputSet[i]);
		node = nodeList[length - i - 1];
		node->delta = errorDerv(node, outputSet[i]);
		if (DEBUG) printNode(node, 1);
		recurDelta(node);
	}

	if (DEBUG) printf("adjusting weights\n");
	//Adjust weights
	for (int i = 0; i < numOutputs; i++)
	{
		node = nodeList[length - i - 1];
		for (int j = 0; j < node->numInputs; j++)
		{
			input = node->inputs[j];
			node->weights[j] -= degree * node->delta * input->out;
			node->relations[j] = TRAINED;

			if (DEBUG) printNode(node, 1);
			trainWeights(input, degree);
		}
	}
}

double train(Node *nodeList[], int numNodes, double *inputSet[], int numInputs,
	double *outputSet[], int numOutputs, int setSize, int(*randFunc)(), double minError,
	int cutOff, double degree)
{
	if (DEBUG) printf("Begin Training\n");
	int count = 0;

	int set;
	double error;

	int complete = 0;
	while (count < cutOff)
	{
		if (DEBUG) printf("Checking network (%d)\n", count);
		//check network against the training set
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
			}

			if (DEBUG) printf("forwards\n");

			//run network forward
			for (int i = 0; i < numNodes; i++)
			{
				if (nodeList[i]->status == OFF)
					forwards(nodeList[i]);
			}

			//calulate error for network
			error = 0.f;
			for (int i = numNodes - numOutputs; i < numNodes; i++)
			{
				int iout = i - (numNodes - numOutputs);
				error += errorNorm(nodeList[i]->out, outputSet[set][iout]);
			}
			if (DEBUG) printf("Set %d error %.3f (%.3f)\n", set, error, minError);
			if (error > minError)
			{
				break; //continue onto training step
			}
			else
			{
				if (set == setSize - 1)
					complete = 1; //minErr met with within all sets
			}
		}
		if (complete) //minErr met with within all sets
			break; //end training

		//train network with random set
		set = randFunc() % setSize;

		if (DEBUG) printf("Training with set %d\n", set);

		//clear network
		for (int i = 0; i < numNodes; i++)
		{
			resetNode(nodeList[i]);
		}

		//load input to nodeList
		for (int i = 0; i < numInputs; i++)
		{
			nodeList[i]->out = inputSet[set][i];
		}

		//run network forward
		for (int i = 0; i < numNodes; i++)
		{
			if (nodeList[i]->status == OFF)
				forwards(nodeList[i]);
		}
		//adjust network
		backwards(nodeList, numNodes, outputSet[set], numOutputs, degree);

		++count;
	}

	fprintf(stderr, "Training stopped %d / %d (%.2f%%)\n", count, cutOff, (double)count / (double)cutOff * 100.f);

	double avgErr = 0;
	for (int set = 0; set < setSize; set++)
	{
		if (count == cutOff)
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
			}

			//run network forward
			for (int i = 0; i < numNodes; i++)
			{
				if (nodeList[i]->status == OFF)
					forwards(nodeList[i]);
			}

			//calulate error for network
			error = 0;
			for (int i = numNodes - numOutputs; i < numNodes; i++)
			{
				int iout = i - (numNodes - numOutputs);
				error += errorNorm(nodeList[i]->out, outputSet[set][iout]);
			}
		}
		avgErr += error;
	}
	avgErr = avgErr / (double)setSize;

	return avgErr;
}