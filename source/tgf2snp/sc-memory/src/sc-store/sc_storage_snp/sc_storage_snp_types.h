#ifndef _sc_storage_snp_types_h_
#define _sc_storage_snp_types_h_

#ifdef ENABLE_HARDWARE_STORAGE

#include "sc_storage_snp_config.h"

extern "C" {
#include "../sc_types.h"
}

namespace sc_storage_snp {

union tVertexID
{
    sc_addr m_asAddr;   // 32bit
    struct {
        uint8 _raw[4];
    } m_asBitfield;     // 32bit
};

struct tVertex
{
    tVertexID           m_ID;               // 32bit
    sc_access_levels    m_scAccessLevels;   // 8bit
    sc_type             m_scType;           // 16bit
    uint8               m_uLock;            // 8bit
    uint8               m_uType     : 2;    // 2bit
    uint8               m_uLinkSize : 5;    // 5bit
    uint16              _reserved   : 9;    // 9bit
    uint16              m_uSearch;          // 16bit
};

struct tEdge
{
    tVertexID   m_ID1;              // 32bit
    tVertexID   m_ID2;              // 32bit
    uint8       m_uType     : 2;    // 2bit
    uint16      _reserved   : 14;   // 11bit
    uint16      m_uSearch;          // 16bit
};

struct tLink
{
    tVertexID   m_ID;                   // 32bit
    uint32      m_uChecksum;            // 32bit
    uint8       m_uType         : 2;    // 2bit
    uint8       m_uLinkIndex    : 3;    // 3bit
    uint16      _reserved       : 11;   // 11bit
    uint16      m_uSearch;              // 16bit
};

enum tCellType
{
    tCellType_EMPTY     = 0x00,
    tCellType_VERTEX    = 0x01,
    tCellType_EDGE      = 0x02,
    tCellType_LINK      = 0x03
};

union tCell
{
    tDevice::tBitfield  m_asBitfield;
    tVertex             m_asVertex;
    tEdge               m_asEdge;
    tLink               m_asLink;
};

}

#endif //ENABLE_HARDWARE_STORAGE

#endif //_sc_storage_snp_types_h_
