#ifndef __GRAPH_H__
#define __GRAPH_H__

/*
 ** CGraph class represents classic bipartite graph in which the path is searched.
 ** As a storage snp device is used which is a wrapper around GPU memory.
 ** Class API is strongly oriented on path-finding algorithm we are using.
 **
 ** There're two basic data types:
 **
 ** 1. Vertex of graph (16bytes = 4uint = 128bits)
 ** +--------+---------+---------+------------+-----------+----------+------------+--------+
 ** |  Type  |   ID    |  prevID | uiPathCost | #reserved | bVisited | bNextFront | bFront |
 ** +--------+---------+---------+------------+-----------+----------+------------+--------+
 ** | 2 bits | 32 bits | 32 bits |  32 bits   |  27 bits  |  1 bit   |   1 bit    | 1 bit  |
 ** +--------+---------+---------+------------+-----------+----------+------------+--------+
 **
 ** 2. Oriented edge of graph (16bytes = 4uint = 128bits)
 ** +--------+---------+---------+------------+-------------------------------+------------+
 ** |  Type  |   ID1   |   ID2   | uiPathCost |          #reserved            | bConnected |
 ** +--------+---------+---------+------------+-------------------------------+------------+
 ** | 2 bits | 32 bits | 32 bits |  32 bits   |           29 bits             |   1 bit    |
 ** +--------+---------+---------+------------+-------------------------------+------------+
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
    bool ResetGraph(); // clear device memory
    bool CreateVertex(uint32 uiID);
    bool CreateEdge(uint32 uiID1, uint32 uiID2, uint32 uiPathCost);

private:
    union tCell
    {
        tBitfield   m_asBitfield;
        tVertex     m_asVertex;
        tEdge       m_asEdge;
    };

    CGraph();

    bool Init(uint32 uiCellsPerPU, uint32 uiNumberOfPU);

    // Forbit parent direct interface
    using snp::tmDevice<CELL_BITWIDTH>::Configure;
    using snp::tmDevice<CELL_BITWIDTH>::End;
    using snp::tmDevice<CELL_BITWIDTH>::Exec;
    using snp::tmDevice<CELL_BITWIDTH>::Read;

//
//	// Graph generator support
//	//bool IsArcExists(uint32 ID1, uint32 ID2);
//
//	
//	// working with vertex table
//	bool setMinDistance(uint32 ID, uint32 prevID, const snpPFDistance &distance);
//	bool setWaveStartVertex(uint32 ID);
//	bool readNextVertexFromWavefront(snpGraphVertex &output);
//	bool readVertex(uint32 ID, snpGraphVertex &output);
//	bool moveWavefront();
//
//	// working with arcs table
//	bool findAllOutputArcs(uint32 ID1);
//	bool readNextOutputArc(snpGraphArc &output);
};

#endif //__GRAPH_H__
