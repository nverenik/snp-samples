#ifndef __TYPES_H__
#define __TYPES_H__

#include <snp/Macros.h>

union tVertexID
{
    uint32  m_asUint32; // 32bit
    struct {
        uint8 raw[4];
    } m_asBitfield;     // 32bit
};

struct tVertex
{
    uint32              m_uiType        : 2;    // 2bits
    tVertexID           m_uiID;                 // 32bits
    tVertexID           m_uiPrevID;             // 32bits
    uint32              m_uiPathCost;           // 32bits
    uint32              _reserved       : 27;   // 27bits
    uint32              m_uiVisited     : 1;    // 1bit
    uint32              m_uiNextFront   : 1;    // 1bit
    uint32              m_uiFront       : 1;    // 1bit
};

struct tEdge
{
    uint32              m_uiType        : 2;    // 2bits
    tVertexID           m_uiIDFrom;             // 32bits
    tVertexID           m_uiIDTo;               // 32bits
    uint32              m_uiPathCost;           // 32bits
    uint32              _reserved       : 29;   // 29bits
    uint32              m_uiConnected   : 1;    // 1bit
};

enum tCellType
{
    tCellType_EMPTY = 0,
    tCellType_VERTEX,
    tCellType_EDGE
};

#endif //__TYPES_H__
