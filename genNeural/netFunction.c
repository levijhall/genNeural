#include "netFunction.h"

#include<string.h>

NetFunction *initFuncList()
{
	NetFunction * funcList = (NetFunction*)malloc(4 * sizeof(NetFunction));

	//manual allocation
	strcpy(funcList[0].name, "ID");
	strcpy(funcList[1].name, "TANH");
	strcpy(funcList[2].name, "SIGM");
	strcpy(funcList[3].name, "SOFT");

	funcList[0].norm = ID;
	funcList[1].norm = TANH;
	funcList[2].norm = SIGMOID;
	funcList[3].norm = SOFTPLUS;

	funcList[0].derv = IDderv;
	funcList[1].derv = TANHderv;
	funcList[2].derv = SIGMOIDderv;
	funcList[3].derv = SOFTPLUSderv;

	return funcList;
}

int getFuncByName(char* name)
{
	if (strcmp(name, "ID") == 0) return 0;
	if (strcmp(name, "TANH") == 0) return 1;
	if (strcmp(name, "SIGM") == 0) return 2;
	if (strcmp(name, "SOFT") == 0) return 3;
	return -1;
}

//ID
double ID(double x)
{
	return x;
}

double IDderv(double x)
{
	return 1.f;
}

//TANH
double TANH(double x)
{
	double t = tanh(x);
	return t / 2.f + 0.5f;
}

double TANHderv(double x)
{
	double t = tanh(x);
	return (1 - t * t) / 2.f;
}

//SIGMOID
double SIGMOID(double x)
{
	return 1.f / (1.f + exp(-x));
}

double SIGMOIDderv(double x)
{
	double e = exp(x);
	double d = 1.f + e;
	return e / d / d;
}

//SOFTPLUS
double SOFTPLUS(double x)
{
	return log(1.f + exp(x));
}

double SOFTPLUSderv(double x)
{
	return SIGMOID(x);
}