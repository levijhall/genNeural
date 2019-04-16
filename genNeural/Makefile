# Levi J. Hall
# ljhall@ucsc.edu

OBJS = main.o netGen.o loadWgtGraph.o node.o netFunction.o loadTrainingSet.o
CC = gcc
CFLAGS = -c -g -O0 -Wall -std=c99 -D_SVID_SOURCE
LFLAGS = -lm
EXE = netGen

all : netGen

netGen: $(OBJS)
		$(CC) $(OBJS) $(LFLAGS) -o $(EXE)

main.o: main.c netGen.o loadWgtGraph.o node.o loadTrainingSet.o
		$(CC) $(CFLAGS) main.c

netGen.o: netGen.c netGen.h node.o
		$(CC) $(CFLAGS) netGen.c

loadWgtGraph.o: loadWgtGraph.c loadWgtGraph.h node.o netFunction.o
		$(CC) $(CFLAGS) loadWgtGraph.c

node.o: node.c node.h netFunction.o
		$(CC) $(CFLAGS) node.c

netFunction.o: netFunction.c netFunction.h
		$(CC) $(CFLAGS) netFunction.c

loadTrainingSet.o: loadTrainingSet.c loadTrainingSet.h
		$(CC) $(CFLAGS) loadTrainingSet.c

clean:
		-rm $(OBJS) $(OBJS2) $(EXE)