/*
Copyright 2019, Anand Patel & Michael Otte

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>


#include "heap.h"
#include "heap.cpp"

#include "graph.h"

// returns random number between 0 and 1
float rand_f()
{
  return (float)rand() / (float)RAND_MAX;
}


// does the node expand opperation (note: we are
// using A*)
void expand(Heap<Node> &H, Node* thisNode, Node* startNode, Node* goalNode)
{
  for(int n = 0; n < thisNode->numOutgoingEdges; n++)
  {
    Edge* thisEdge = thisNode->outgoingEdges[n];  // pointer to this edge
    Node* neighborNode = thisEdge->endNode;  // pointer to the node on the
                                             // other end of this edge
	double cost2neighbor = thisEdge->edgeCost;	// cost(thisNode, neighborNode)

    if((neighborNode->status == 0) || (neighborNode->cost2start > (thisNode->cost2start + cost2neighbor)))  // neighbor has not yet been visited OR cost2start ineq satisfied
    {

      // remember this node as its parent
      neighborNode->parentNode = thisNode;

	  // recalculate neighbor node's cost2start
	  neighborNode->cost2start = thisNode->cost2start + cost2neighbor;

	  // calculating neighbor node's cost2goal heuristically
	  double x1 = neighborNode->x;
	  double y1 = neighborNode->y;
	  double x2 = goalNode->x;
	  double y2 = goalNode->y;

	  double H_u2goal = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	  neighborNode->cost2goal = H_u2goal;

	  //update neighbor in heap (with cost2start+cost2goal as key)
	  H.updateNodeInHeap(neighborNode, neighborNode->cost2start + neighborNode->cost2goal);

      // make sure it is in the open list
      neighborNode->status = 1;
    }
  }

  thisNode->status = 2;    // now this node is in the closed list
}



int main()
{
  srand(time(NULL)); // seed random number generator

  Graph G;
  G.readGraphFromFiles("nodes.txt", "edges.txt");
//  G.printGraph();

  // we want to find a path that goes from here to here
  int startNodeIndex = 7-1;
  int goalNodeIndex = 18-1;
  //int startNodeIndex = 1;
  //int goalNodeIndex = 100;


  Heap<Node> H(100); // this is the heap (start's with space for 100 items
                     // but will grow automatically as needed).


  // these are pointers to the start and end nodes
  Node* startNode = &G.nodes[startNodeIndex];
  Node* goalNode = &G.nodes[goalNodeIndex];

  // cost2start of the start node is always 0
  startNode->cost2start = 0;

  // marker to indicate success (true) or failure (false). Default is failure.
  bool marker = false;

  // we'll do a random walk search, but we'll do it
  // by assigning nodes a random key in the heap
  // this way it can be showen how the heap works

  // this is left unchanged from random walk code b/c it does
  // not matter what the key of the startnode is since it is
  // pop'd first and closed

  double key = rand_f();
  H.addToHeap(startNode, key);
  startNode->status = 1;    // now the start node is in the open list

  // while there are nodes left in the heap
  // (note that a better stopping criteria should be used in
  // most "real" algorithms, but this will cause us to generate
  // a random spanning tree over the graph, which is kind of cool)
  while(H.topHeap() != NULL)
  {
    Node* thisNode = H.popHeap();
    expand(H, thisNode, startNode, goalNode);

	if (thisNode->id == goalNode->id)
	{
		marker = true;		// SUCCESS in finding goal node
		break;				// Break out of while loop
	}
    // H.printHeap();
  }

  if (marker)
  {
	  printf("SUCCESS\n");
  }
  else
  {
	  printf("FAILURE\n");
  }

  // now we want to save files that contain the search tree that we build
  // and also the best path that we found (NOTE, these add 1 to indicies
  // to make them compativle with the graph file that was used as input for
  // the search)

  G.savePathToFile("output_path.txt", goalNode);
  G.saveSearchTreeToFile("search_tree.txt");


  return 0;
}
