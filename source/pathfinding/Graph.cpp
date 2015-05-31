#include "Graph.h"
using namespace snp;

#define SNP_BITFIELD_DATA_SET                   snpBitfieldSet
#define SNP_BITFIELD_MASK_SET(__bitfield__)     snpBitfieldSet((__bitfield__), -1)
#define SNP_BITFIELD_MASK_RESET(__bitfield__)   snpBitfieldSet((__bitfield__),  0)

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
            printf("WARNING: error during shutting down.");
    }
}

bool CGraph::Init(uint32 uiCellsPerPU, uint32 uiNumberOfPU)
{
    if (Configure(uiCellsPerPU, uiNumberOfPU) != CErrorCode::SUCCEEDED)
        return false;

    return true;
}

bool CGraph::ResetGraph()
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
    SNP_BITFIELD_DATA_SET(oWriteData.m_asBitfield._raw, 0);
    oWriteData.m_asVertex.m_uiType = tCellType_VERTEX;
    oWriteData.m_asVertex.m_uiID.m_asUint32 = uiID;

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

bool CGraph::CreateEdge(uint32 uiID1, uint32 uiID2, uint32 uiPathCost)
{
    tCell oSearchMask;
    SNP_BITFIELD_MASK_RESET(oSearchMask.m_asBitfield._raw);
    oSearchMask.m_asEdge.m_uiType = -1;

    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uiType = tCellType_EMPTY;

    tCell oWriteMask;
    SNP_BITFIELD_MASK_SET(oWriteMask.m_asBitfield._raw);

    tCell oWriteData;
    SNP_BITFIELD_DATA_SET(oWriteData.m_asBitfield._raw, 0);
    oWriteData.m_asEdge.m_uiType = tCellType_EDGE;
    oWriteData.m_asEdge.m_uiIDFrom.m_asUint32 = uiID1;
    oWriteData.m_asEdge.m_uiIDTo.m_asUint32 = uiID2;
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
