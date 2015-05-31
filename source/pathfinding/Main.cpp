#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"

int main(int argc, char* argv[])
{
    CGraph::SP pGraph = CGraph::Create(32, 32);
    pGraph->ResetGraph();

	return 0;
}
