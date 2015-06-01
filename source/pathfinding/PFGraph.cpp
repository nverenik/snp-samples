#include "PFGraph.h"
using namespace snp;

#define SNP_BITFIELD_DATA_SET                   snpBitfieldSet
#define SNP_BITFIELD_MASK_SET(__bitfield__)     snpBitfieldSet((__bitfield__), -1)
#define SNP_BITFIELD_MASK_RESET(__bitfield__)   snpBitfieldSet((__bitfield__),  0)

CGraph::tCell::tCell()
{
    SNP_BITFIELD_DATA_SET(m_asBitfield._raw, 0);
}

CGraph::SP CGraph::Create(uint32 uiCellsPerPU, uint32 uiNumberOfPU)
{
    CGraph *pGraph = new CGraph();
    if (pGraph && pGraph->Init(uiCellsPerPU, uiNumberOfPU))
        return CGraph::SP(pGraph);

    delete pGraph;
    return CGraph::SP();
}

CGraph::CGraph()
{
}

CGraph::~CGraph()
{
    if (IsReady())
    {
        CErrorCode eErrorCode = End(); // not necessary but to be sure
        if (eErrorCode != CErrorCode::SUCCEEDED)
            printf("ERROR: CGraph::~CGraph() - error [%d] during shutting down.\n", eErrorCode);
    }
}

bool CGraph::Init(uint32 uiCellsPerPU, uint32 uiNumberOfPU)
{
    if (Configure(uiCellsPerPU, uiNumberOfPU) != CErrorCode::SUCCEEDED)
        return false;

    return true;
}

bool CGraph::Clear()
{
    tInstruction oInstruction;
    SNP_BITFIELD_MASK_RESET(oInstruction.m_oSearchMask._raw);   // do not test any of bits so all cells will be addressed
    SNP_BITFIELD_MASK_SET(oInstruction.m_oWriteMask._raw);
    SNP_BITFIELD_DATA_SET(oInstruction.m_oWriteData._raw, 0);   // clear all memory, but it's actually enough to just clear type field

    CErrorCode eErrorCode;
    bool bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::ResetGraph() - error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}

bool CGraph::CreateVertex(uint32 uiID)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asVertex.m_uiType = -1;

    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_EMPTY;

    tCell oWriteMask;
    SNP_BITFIELD_MASK_SET(oWriteMask.m_asBitfield._raw);

    tCell oWriteData;
    oWriteData.m_asVertex.m_uiType = tCellType_VERTEX;
    oWriteData.m_asVertex.m_uiID = uiID;

    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bCreated = Exec(true, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::CreateVertex() - error code [%d]\n", eErrorCode);
        return false;
    }
    return bCreated;
}

bool CGraph::CreateEdge(uint32 uiIDFrom, uint32 uiIDTo, uint32 uiPathCost)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asEdge.m_uiType = -1;

    tCell oSearchTag;
    oSearchTag.m_asEdge.m_uiType = tCellType_EMPTY;

    tCell oWriteMask;
    SNP_BITFIELD_MASK_SET(oWriteMask.m_asBitfield._raw);

    tCell oWriteData;
    oWriteData.m_asEdge.m_uiType = tCellType_EDGE;
    oWriteData.m_asEdge.m_uiIDFrom = uiIDFrom;
    oWriteData.m_asEdge.m_uiIDTo = uiIDTo;
    oWriteData.m_asEdge.m_uiPathCost = uiPathCost;

    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bCreated = Exec(true, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::CreateArc() - error code [%d]\n", eErrorCode);
        return false;
    }
    return bCreated;
}

bool CGraph::ResetGraph()
{
    // Reset vertices
    bool bResult = false;
    {
        tCell oSearchMask;
        SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
        oSearchMask.m_asVertex.m_uiType = -1;
        
        tCell oSearchTag;
        oSearchTag.m_asVertex.m_uiType = tCellType_VERTEX;
        
        tCell oWriteMask;
        SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
        oWriteMask.m_asVertex.m_uiPrevID = -1;
        oWriteMask.m_asVertex.m_uiPathCost = -1;
        oWriteMask.m_asVertex.m_uiVisited = -1;
        oWriteMask.m_asVertex.m_uiNextFront = -1;
        oWriteMask.m_asVertex.m_uiFront = -1;
        
        tInstruction oInstruction;
        oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
        oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
        oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
        
        CErrorCode eErrorCode;
        bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode) || bResult;
        if (eErrorCode != CErrorCode::SUCCEEDED)
        {
            printf("ERROR: CGraph::ResetGraph() - resetting vertices, error code [%d]\n", eErrorCode);
            return false;
        }
    };
    // Reset edges
    {
        tCell oSearchMask;
        SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
        oSearchMask.m_asEdge.m_uiType = -1;
        
        tCell oSearchTag;
        oSearchTag.m_asEdge.m_uiType = tCellType_EDGE;
        
        tCell oWriteMask;
        SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
        oWriteMask.m_asEdge.m_uiConnected = -1;
        
        tInstruction oInstruction;
        oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
        oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
        oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
        
        CErrorCode eErrorCode;
        bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode) || bResult;
        if (eErrorCode != CErrorCode::SUCCEEDED)
        {
            printf("ERROR: CGraph::ResetGraph() - resetting edges, error code [%d]\n", eErrorCode);
            return false;
        }
    };
    return bResult;
}

bool CGraph::SetMinPathCost(uint32 uiID, uint32 uiPrevID, uint32 uiPathCost)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asVertex.m_uiType = -1;
    oSearchMask.m_asVertex.m_uiID = -1;

    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_VERTEX;
    oSearchTag.m_asVertex.m_uiID = uiID;

    tCell oWriteMask;
    SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
    oWriteMask.m_asVertex.m_uiPrevID = -1;
    oWriteMask.m_asVertex.m_uiPathCost = -1;
    oWriteMask.m_asVertex.m_uiVisited = -1;
    oWriteMask.m_asVertex.m_uiNextFront = -1;
    oWriteMask.m_asVertex.m_uiFront = -1;

    tCell oWriteData;
    oWriteMask.m_asVertex.m_uiPrevID = uiPrevID;
    oWriteMask.m_asVertex.m_uiPathCost = uiPathCost;
    oWriteMask.m_asVertex.m_uiVisited = 1;
    oWriteMask.m_asVertex.m_uiNextFront = 1;
    oWriteMask.m_asVertex.m_uiFront = 0;

    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::SetMinPathCost() - error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}

bool CGraph::SetWaveStartVertex(uint32 uiID)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asVertex.m_uiType = -1;
    oSearchMask.m_asVertex.m_uiID = -1;
    
    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_VERTEX;
    oSearchTag.m_asVertex.m_uiID = uiID;
    
    tCell oWriteMask;
    SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
    oWriteMask.m_asVertex.m_uiPrevID = -1;
    oWriteMask.m_asVertex.m_uiPathCost = -1;
    oWriteMask.m_asVertex.m_uiVisited = -1;
    oWriteMask.m_asVertex.m_uiNextFront = -1;
    oWriteMask.m_asVertex.m_uiFront = -1;

    tCell oWriteData;
    oWriteData.m_asVertex.m_uiPrevID = uiID; // itself
    oWriteData.m_asVertex.m_uiPathCost = 0;
    oWriteData.m_asVertex.m_uiVisited = 1;
    oWriteData.m_asVertex.m_uiNextFront = 0;
    oWriteData.m_asVertex.m_uiFront = 1;
    
    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::SetWaveStartVertex() - error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}

bool CGraph::ReadNextVertexFromWavefront(tVertex &roVertex)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asVertex.m_uiType = -1;
    oSearchMask.m_asVertex.m_uiFront = -1;
    
    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_VERTEX;
    oSearchTag.m_asVertex.m_uiFront = 1;
    
    tCell oWriteMask;
    SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
    oWriteMask.m_asVertex.m_uiFront = -1;

    tCell oWriteData;
    oWriteData.m_asVertex.m_uiFront = 0;
    
    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bResult = Exec(true, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::MoveWavefront() - finding vertex, error code [%d]\n", eErrorCode);
        return false;
    }
    if (!bResult) return false;

    tCell *pCell = (tCell *)&roVertex;
    bResult = Read((*pCell).m_asBitfield, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::MoveWavefront() - reading vertex, error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}

bool CGraph::ReadVertex(uint32 uiID, tVertex &roVertex)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asVertex.m_uiType = -1;
    oSearchMask.m_asVertex.m_uiID = -1;
    
    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_VERTEX;
    oSearchTag.m_asVertex.m_uiID = uiID;
    
    tCell oWriteMask;
    SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
    
    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;

    CErrorCode eErrorCode;
    bool bResult = Exec(true, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::ReadVertex() - finding vertex, error code [%d]\n", eErrorCode);
        return false;
    }
    if (!bResult) return false;

    tCell *pCell = (tCell *)&roVertex;
    bResult = Read((*pCell).m_asBitfield, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::MoveWavefront() - reading vertex, error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}

bool CGraph::MoveWavefront()
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asVertex.m_uiType = -1;
    oSearchMask.m_asVertex.m_uiNextFront = -1;
    
    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_VERTEX;
    oSearchTag.m_asVertex.m_uiNextFront = 1;
    
    tCell oWriteMask;
    SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
    oWriteMask.m_asVertex.m_uiNextFront = -1;
    oWriteMask.m_asVertex.m_uiFront = -1;

    tCell oWriteData;
    oWriteData.m_asVertex.m_uiNextFront = 0;
    oWriteData.m_asVertex.m_uiFront = 1;
    
    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::MoveWavefront() - error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}

bool CGraph::FindAllOutputEdges(uint32 uiIDFrom)
{
    bool bResult = false;
    // Reset 'bConnected' bit for all edges in the graph
    {
        tCell oSearchMask;
        SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
        oSearchMask.m_asEdge.m_uiType = -1;
    
        tCell oSearchTag;
        oSearchTag.m_asEdge.m_uiType = tCellType_EDGE;
	    
        tCell oWriteMask;
        SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
        oWriteMask.m_asEdge.m_uiConnected = -1;

        tCell oWriteData;
        oWriteData.m_asEdge.m_uiConnected = 0;

        tInstruction oInstruction;
        oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
        oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
        oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
        oInstruction.m_oWriteData = oWriteData.m_asBitfield;

        CErrorCode eErrorCode;
        bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode);
        if (eErrorCode != CErrorCode::SUCCEEDED)
        {
            printf("ERROR: CGraph::FindAllOutputEdges() - resetting state, error code [%d]\n", eErrorCode);
            return false;
        }

        // If there're no edges were found we can return safely
        if (!bResult) return false;
    };
    // Mark all output edges by 'bConnected' bit
    {
        tCell oSearchMask;
        SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
        oSearchMask.m_asEdge.m_uiType = -1;
        oSearchMask.m_asEdge.m_uiIDFrom = -1;

        tCell oSearchTag;
        oSearchTag.m_asEdge.m_uiType = tCellType_EDGE;
        oSearchTag.m_asEdge.m_uiIDFrom = uiIDFrom;

        tCell oWriteMask;
        SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
        oWriteMask.m_asEdge.m_uiConnected = -1;

        tCell oWriteData;
        oWriteData.m_asEdge.m_uiConnected = 1;

        tInstruction oInstruction;
        oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
        oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
        oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
        oInstruction.m_oWriteData = oWriteData.m_asBitfield;

        CErrorCode eErrorCode;
        bResult = Exec(false, tOperation_Assign, oInstruction, &eErrorCode);
        if (eErrorCode != CErrorCode::SUCCEEDED)
        {
            printf("ERROR: CGraph::FindAllOutputEdges() - finding edges, error code [%d]\n", eErrorCode);
            return false;
        }
    };
    return bResult;
}

bool CGraph::ReadNextOutputEdge(tEdge &roEdge)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asEdge.m_uiType = -1;
    oSearchMask.m_asEdge.m_uiConnected = -1;

    tCell oSearchTag;
    oSearchTag.m_asEdge.m_uiType = tCellType_EDGE;
    oSearchTag.m_asEdge.m_uiConnected = 1;

    tCell oWriteMask;
    SNP_BITFIELD_MASK_RESET(oWriteMask.m_asBitfield._raw);
    oWriteMask.m_asEdge.m_uiConnected = -1;

    tCell oWriteData;
    oWriteData.m_asEdge.m_uiConnected = 0;

    tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode eErrorCode;
    bool bResult = Exec(true, tOperation_Assign, oInstruction, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::ReadNextOutputEdge() - finding edge, error code [%d]\n", eErrorCode);
        return false;
    }
    if (!bResult) return false;

    tCell *pCell = (tCell *)&roEdge;
    bResult = Read((*pCell).m_asBitfield, &eErrorCode);
    if (eErrorCode != CErrorCode::SUCCEEDED)
    {
        printf("ERROR: CGraph::ReadNextOutputEdge() - reading edge, error code [%d]\n", eErrorCode);
        return false;
    }
    return bResult;
}
