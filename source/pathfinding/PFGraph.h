#ifndef __PF_GRAPH_H__
#define __PF_GRAPH_H__

/*
 ** CGraph class represents classic bipartite graph in which the path is searched.
 ** As a storage snp device is used which is a wrapper around GPU memory.
 ** Class API is strongly oriented on path-finding algorithm we are using.
 **
 ** There're two basic data types:
 **
 ** 1. Vertex of graph (16bytes = 4uint = 128bits)
 ** +---------+---------+------------+-----------+----------+------------+--------+--------+
 ** |   ID    |  prevID | uiPathCost | #reserved | bVisited | bNextFront | bFront |  Type  |
 ** +---------+---------+------------+-----------+----------+------------+--------+--------+
 ** | 32 bits | 32 bits |  32 bits   |  27 bits  |  1 bit   |   1 bit    | 1 bit  | 2 bits |
 ** +---------+---------+------------+-----------+----------+------------+--------+--------+
 **
 ** 2. Oriented edge of graph (16bytes = 4uint = 128bits)
 ** +---------+---------+------------+-------------------------------+------------+--------+
 ** |   ID1   |   ID2   | uiPathCost |          #reserved            | bConnected |  Type  |
 ** +---------+---------+------------+-------------------------------+------------+--------+
 ** | 32 bits | 32 bits |  32 bits   |           29 bits             |   1 bit    | 2 bits |
 ** +---------+---------+------------+-------------------------------+------------+--------+
 */ 

#include <snp/snp.h>
#include <memory>

#include "Types.h"

#define CELL_BITWIDTH   128

class CGraph
    : snp::tmDevice<CELL_BITWIDTH>
{
public:
    typedef std::shared_ptr<CGraph> SP;

    static CGraph::SP Create(uint32 uiCellsPerPU, uint32 uiNumberOfPU);
    virtual ~CGraph();

    // Initialization
    bool Clear(); // clear the whole device memory
    bool CreateVertex(uint32 uiID);
    bool CreateEdge(uint32 uiIDFrom, uint32 uiIDTo, uint32 uiPathCost);

    // Path-finding specific interface
    bool ResetGraph(); // reset all service information in the graph

    // Working with vertices
    bool SetMinPathCost(uint32 uiID, uint32 uiPrevID, uint32 uiPathCost);
    bool SetWaveStartVertex(uint32 uiID);
    bool ReadNextVertexFromWavefront(tVertex &roVertex);
    bool ReadVertex(uint32 uiID, tVertex &roVertex);
    bool MoveWavefront();

    // Working with edges
    bool FindAllOutputEdges(uint32 uiIDFrom);
    bool ReadNextOutputEdge(tEdge &roEdge);

private:
    union tCell
    {
        tBitfield   m_asBitfield;
        tVertex     m_asVertex;
        tEdge       m_asEdge;

        tCell();
    };

    CGraph();
    bool Init(uint32 uiCellsPerPU, uint32 uiNumberOfPU);

    // Forbit parent direct interface
    using snp::tmDevice<CELL_BITWIDTH>::Configure;
    using snp::tmDevice<CELL_BITWIDTH>::End;
    using snp::tmDevice<CELL_BITWIDTH>::Exec;
    using snp::tmDevice<CELL_BITWIDTH>::Read;
};

#endif //__PF_GRAPH_H__
