///////////////////////////////////////////////////////////////////////
// File:	sc_storage_snp_types.pure.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	semantic network processor pure implementation

#ifndef _sc_storage_snp_types_pure_h_
#define _sc_storage_snp_types_pure_h_

#ifdef ENABLE_HARDWARE_STORAGE

#include "sc_storage_snp_config.pure.h"

extern "C" {
#include "../sc_types.h"
}

namespace sc_storage_snp {

union VertexID
{
    sc_addr m_asAddr;   // 32bit
    struct {
        uint8 raw[4];
    } m_asBitfield;     // 32bit
};

struct Vertex
{
    VertexID            m_ID;               // 32bit
    sc_access_levels    m_scAccessLevels;   // 8bit
    sc_type             m_scType;           // 16bit
    uint8               m_uLock;            // 8bit
    uint8               m_uType     : 2;    // 2bit
    uint8               m_uLinkSize : 5;    // 5bit
    uint16              _reserved   : 9;    // 9bit
    uint16              m_uSearch;          // 16bit
};

struct Edge
{
    VertexID    m_ID1;              // 32bit
    VertexID    m_ID2;              // 32bit
    uint8       m_uType     : 2;    // 2bit
    uint16      _reserved   : 14;   // 11bit
    uint16      m_uSearch;          // 16bit
};

struct Link
{
    VertexID    m_ID;                   // 32bit
    uint32      m_uChecksum;            // 32bit
    uint8       m_uType         : 2;    // 2bit
    uint8       m_uLinkIndex    : 3;    // 3bit
    uint16      _reserved       : 11;   // 11bit
    uint16      m_uSearch;              // 16bit
};

enum
#ifndef PURE_IMPLEMENTATION
class
#endif//PURE_IMPLEMENTATION
	CellType 
#ifndef PURE_IMPLEMENTATION
: uint8
#endif//PURE_IMPLEMENTATION
{
    EMPTY = 0,
    VERTEX,
    EDGE,
    LINK
};

union Cell
{
#ifndef PURE_IMPLEMENTATION
    Device::snpBitfield m_asBitfield;
#else
	uint32	bf[3];
#endif//PURE_IMPLEMENTATION
    Vertex              m_asVertex;
    Edge                m_asEdge;
    Link                m_asLink;
};

}

#endif //ENABLE_HARDWARE_STORAGE

#endif //_sc_storage_snp_types_pure_h_
