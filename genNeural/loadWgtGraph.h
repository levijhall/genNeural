/* Levi J. Hall
 * ljhall@ucsc.edu
 */

#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "netFunction.h"

 /* loadWgtGraph.h
  * Providing a prototype to parse a graph and store the result
  *    vertex points begin at index 1
  */

#ifndef C101LoadWgtGraph
#define C101LoadWgtGraph

  /**********************************************************************
   * CONSTRUCTORS
   */

   /** parseEdges()
	*  preconditions: none.
	*  postconditions: Let /return be the value returned by parseEdges(fp, length)
	*      If an error is incountered, /return = NULL
	*      Otherwise length(/return) = length
	*/
Node **parseEdges(FILE *fp, int *length, double(*randFunc)());

/**********************************************************************
 * Manipulation
 */

 /** printNodeList()
  *  preconditions: nodeList has been constructed.
  *  postconditions: print to stdout a formatted representation of the
  *      a list of adjacent vertices.
  */
void printNodeList(Node **nodeList, int numNodes);

#endif //C101LoadWgtGraph