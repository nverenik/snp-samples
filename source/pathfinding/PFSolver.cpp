#include "PFSolver.h"
#include "PFGraph.h"

bool PFSolver::FindPath(CGraph *pGraph, uint32 uiSrcID, uint32 uiDstID)
{
    if (!pGraph)
    {
        printf("ERROR: PFSolver::FindPath() - graph can't be null.\n");
        return false;
    }

    pGraph->ResetGraph();
    if (!pGraph->SetWaveStartVertex(uiSrcID))
    {
        printf("ERROR: PFSolver::FindPath() - src vertex not found.\n");
        return false;
    }
    
    while (true)
    {
        tVertex oSrcVertex;
        while (pGraph->ReadNextVertexFromWavefront(oSrcVertex))
        {
            pGraph->FindAllOutputEdges(oSrcVertex.m_uiID);

            tEdge oEdge;
            while(pGraph->ReadNextOutputEdge(oEdge))
            {
                tVertex oDstVertex;
                if (!pGraph->ReadVertex(oEdge.m_uiIDTo, oDstVertex))
                {
                    printf("WARNING: PFSolver::FindPath() - incidence edge's vertex not found.\n", uiSrcID);
                    continue;
                }

                uint32 uiPathCost = oSrcVertex.m_uiPathCost + oEdge.m_uiPathCost;
                if (!oDstVertex.m_uiVisited || uiPathCost < oDstVertex.m_uiPathCost)
                    pGraph->SetMinPathCost(oDstVertex.m_uiID, oSrcVertex.m_uiID, uiPathCost);
            }
        }

        if (!pGraph->MoveWavefront())
            break;
	}

    // Check result
    tVertex oDstVertex;
    if (pGraph->ReadVertex(uiDstID, oDstVertex))
        return (oDstVertex.m_uiVisited != 0);

    printf("ERROR: PFSolver::FindPath() - dst vertex not found.\n");
    return false;
}
