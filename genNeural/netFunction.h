#include <math.h>
#include <stdlib.h>

#ifndef C101netFunction
#define C101netFunction

typedef struct netFunction
{
	char name[4];
	double(*norm)(double);
	double(*derv)(double);
} NetFunction;

NetFunction *initFuncList(void);

int getFuncByName(char* name);

//name ID, identity function
double ID(double x);
double IDderv(double x);

//name TANH
double TANH(double x);
double TANHderv(double x);

//name SIGM
double SIGMOID(double x);
double SIGMOIDderv(double x);

//name SOFT
double SOFTPLUS(double x);
double SOFTPLUSderv(double x);

#endif //C101netFunction