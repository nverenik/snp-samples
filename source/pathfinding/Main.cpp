#include <stdio.h>
#include <stdlib.h>

#include "PFGraph.h"
#include <Pigale.h>

const uint32 s_uiMaxPathCost = 100;

inline int pow2roundup (int x)
{
    if (x < 0) return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

int main(int argc, char* argv[])
{
    // It's must be enough to just use time function as a duration spent to work with device is significant
    randomInitSeed();
	
    enum tGraphType
    {
        tGraphType_Planar       = 1,    // allowed connectivity: 1, 2, 3
        tGraphType_Bipartite    = 2,    // allowed connectivity: 2, 3, 4
        tGraphType_Cubic        = 3,    // allowed connectivity: 2, 4, 6, 0
        tGraphType_Quadric      = 4     // allowed connectivity: 1, 2, 3
    };

    // Generate graph
    GraphContainer *pGraphContainer = GenerateSchaeffer(1000 /*edges*/, tGraphType_Planar, 3, true, false);
    GeometricGraph oGeometricGraph(*pGraphContainer);

    // Store graph configuration into device memory
    uint32 aaa = oGeometricGraph.nv();
    uint32 bbb = oGeometricGraph.ne();

    const uint32 uiNumberOfCells = pow2roundup(oGeometricGraph.nv() + oGeometricGraph.ne());
    CGraph::SP pGraph = CGraph::Create(1, uiNumberOfCells); //fastest configuration - 1 thread per 1 cell

    pGraph->ResetGraph();
    for (uint32 uiIndex = 1; uiIndex <= oGeometricGraph.nv(); uiIndex++)
        pGraph->CreateVertex(uiIndex);

    Prop<tvertex> vin(oGeometricGraph.Set(tbrin()),PROP_VIN);
    Prop<long> vlabel(oGeometricGraph.Set(tvertex()), PROP_LABEL);
    
    for (int iIndex = 0; iIndex <= oGeometricGraph.nv(); iIndex++)
        vlabel[iIndex] = iIndex;
    
    for (int edge = 1; edge <= oGeometricGraph.ne(); edge++)
        pGraph->CreateEdge(vlabel[vin[(tbrin)edge]()], vlabel[vin[(tbrin)-edge]()], rand() % s_uiMaxPathCost);
	
	delete pGraphContainer;
    return 0;
}
