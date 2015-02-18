///////////////////////////////////////////////////////////////////////
// File:	sc_storage_snp_glue.pure.cpp
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	semantic network processor pure implementation

#include "sc_storage_snp_glue.h"

#ifndef assert
#define assert(x)
#endif//assert
#ifndef nullptr
#define nullptr NULL
#endif//nullptr

#ifdef ENABLE_HARDWARE_STORAGE

#include <string>
#include <vector>

#include <snp/snp.h>
using snp::CErrorCode;

#include "sc_storage_snp_types.h"
using namespace sc_storage_snp;

// create static device instance with predefined bitwidth
static tDevice s_oDevice;

static std::string s_sDumpFilePath;
static std::string s_sDumpFileName = "snp.dump";

// forward declaration for private methods
void snp_enter_critical_section();
void snp_leave_critical_section();

tVertexID snp_vertex_id_obtain();

bool snp_vertex_create(const tVertexID &vertex_id, sc_type type, sc_access_levels access_levels);
bool snp_edge_create(const tVertexID &vertex_id1, const tVertexID &vertex_id2);

#define SNP_BITFIELD_DATA_SET                   snpBitfieldSet
#define SNP_BITFIELD_MASK_SET(__bitfield__)     snpBitfieldSet((__bitfield__), -1)
#define SNP_BITFIELD_MASK_RESET(__bitfield__)   snpBitfieldSet((__bitfield__),  0)

sc_bool snp_initialize(const char *path, sc_bool clear)
{
    snp_enter_critical_section();

    bool bResult = false;
    do                   
    {
        //// dump file is mandatory
        //if (!path || !strlen(path))
        //    break;

        // initialize/configure snp device (single GPU or cluster)
        CErrorCode eResult = s_oDevice.Configure(g_iCellsPerPU, g_iNumberOfPU);
        if (eResult != CErrorCode::SUCCEEDED)
        {
            // print error log accordingly to returned code
            // for now just assert in any case
            assert(0);
            break;
        }

        if (/*clear*/ true /* ... for now */)
        {
            // todo: remove file if it exists
            // ...

            // clear device memory
            tDevice::tInstruction oInstruction;
            SNP_BITFIELD_MASK_RESET(oInstruction.m_oSearchMask._raw);
            SNP_BITFIELD_MASK_SET(oInstruction.m_oWriteMask._raw);
            SNP_BITFIELD_DATA_SET(oInstruction.m_oWriteData._raw, 0);
            s_oDevice.Exec(false, snp::tOperation_Assign, oInstruction);
        }
        else
        {
            // todo: try to load file into device memory as is (note
            // that device is not support dumping yet)
        }

        //s_sDumpFilePath = path;
        bResult = true;
    }
    while(0);

    snp_leave_critical_section();
    return bResult ? SC_TRUE : SC_FALSE;
}

void snp_shutdown(sc_bool save_state)
{
    snp_enter_critical_section();
    if (save_state && !s_sDumpFilePath.empty())
    {
        // dump device memory content onto disk
        // in case of cluster should we separate the data on
        // several dump files? like 20 files * 512Mb or smth.

        // note that data size is not related to graph state,
        // it always equals to the size of device memory

        // but we can implement some optimization to store zero fields
        // (skip unused cells and store just used cells)
    }

    // release device
    s_sDumpFilePath.clear();
    s_oDevice.End();

    snp_leave_critical_section();
}

sc_bool snp_is_initialized()
{
    snp_enter_critical_section();
    bool bReady = s_oDevice.IsReady();
    snp_leave_critical_section();
    return bReady ? SC_TRUE : SC_FALSE;
}

sc_addr snp_element_create_node(sc_type type, sc_access_levels access_levels)
{
    snp_enter_critical_section();

    assert(!(sc_type_arc_mask & type));
    type = sc_flags_remove(sc_type_node | type);

    tVertexID tVertexID = snp_vertex_id_obtain();
    bool bCreated = snp_vertex_create(tVertexID, type, access_levels);
    if (!bCreated)
    {
        assert(0);
        SNP_BITFIELD_DATA_SET(tVertexID.m_asBitfield._raw, 0);
    }

    snp_leave_critical_section();
    return tVertexID.m_asAddr;
}
//
sc_addr snp_element_create_arc(sc_type type, sc_addr beg, sc_addr end, sc_access_levels access_levels)
{
    snp_enter_critical_section();

    assert(!(sc_type_node & type));
    type = sc_flags_remove((type & sc_type_arc_mask) ? type : (sc_type_arc_common | type));

    // each arc element is transformed into vertex (which stores metadata)
    // and 2 edges to save direction of connection

    bool bError = false;
    tVertexID oVertexID = snp_vertex_id_obtain();
    do
    {
        // create vertex at first
        bool bCreated = snp_vertex_create(oVertexID, type, access_levels);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }

        // then create edges to not lose connection
        tVertexID oVertexBegin;
        oVertexBegin.m_asAddr = beg;

        bCreated = snp_edge_create(oVertexBegin, oVertexID);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }

        tVertexID oVertexEnd;
        oVertexEnd.m_asAddr = end;

        bCreated = snp_edge_create(oVertexID, oVertexEnd);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }
    }
    while(0);

    if (bError)
    {
        // todo: remove element and release ID
        SNP_BITFIELD_DATA_SET(oVertexID.m_asBitfield._raw, 0);
    }

    snp_leave_critical_section();
    return oVertexID.m_asAddr;
}

sc_addr snp_element_create_arc_begin(sc_type type, sc_addr beg, sc_access_levels access_levels)
{
    snp_enter_critical_section();

    assert(!(sc_type_node & type));
    type = sc_flags_remove((type & sc_type_arc_mask) ? type : (sc_type_arc_common | type));

    // each arc element is transformed into vertex (which stores metadata)
    // and 2 edges to save direction of connection

    bool bError = false;
    tVertexID oVertexID = snp_vertex_id_obtain();
    do
    {
        // create vertex at first
        bool bCreated = snp_vertex_create(oVertexID, type, access_levels);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }

        // then create edges to not lose connection
        tVertexID oVertexBegin;
        oVertexBegin.m_asAddr = beg;

        bCreated = snp_edge_create(oVertexBegin, oVertexID);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }
    }
    while(0);

    if (bError)
    {
        // todo: remove element and release ID
        SNP_BITFIELD_DATA_SET(oVertexID.m_asBitfield._raw, 0);
    }

    snp_leave_critical_section();
    return oVertexID.m_asAddr;
}

sc_addr snp_element_create_arc_loop(sc_type type, sc_addr beg, sc_access_levels access_levels)
{
    snp_enter_critical_section();

    assert(!(sc_type_node & type));
    type = sc_flags_remove((type & sc_type_arc_mask) ? type : (sc_type_arc_common | type));

    // each arc element is transformed into vertex (which stores metadata)
    // and 2 edges to save direction of connection

    bool bError = false;
    tVertexID oVertexID = snp_vertex_id_obtain();
    do
    {
        // create vertex at first
        bool bCreated = snp_vertex_create(oVertexID, type, access_levels);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }

        // then create edges to not lose connection

        tVertexID oVertexBegin;
        oVertexBegin.m_asAddr = beg;

        bCreated = snp_edge_create(oVertexBegin, oVertexID);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }

        tVertexID oVertexEnd;
        oVertexEnd.m_asAddr = oVertexID.m_asAddr;

        bCreated = snp_edge_create(oVertexID, oVertexEnd);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }
    }
    while(0);

    if (bError)
    {
        // todo: remove element and release ID
        SNP_BITFIELD_DATA_SET(oVertexID.m_asBitfield._raw, 0);
    }

    snp_leave_critical_section();
    return oVertexID.m_asAddr;
}

sc_addr snp_element_set_arc_end(sc_addr arc, sc_addr end, sc_access_levels access_levels)
{
    snp_enter_critical_section();

    // each arc element is transformed into vertex (which stores metadata)
    // and 2 edges to save direction of connection

    bool bError = false;
    tVertexID oVertexID;
    oVertexID.m_asAddr = arc;
    do
    {
        tVertexID oVertexEnd;
        oVertexEnd.m_asAddr = end;

        bool bCreated = snp_edge_create(oVertexID, oVertexEnd);
        if (!bCreated)
        {
            assert(0);
            bError = true;
            break;
        }
    }
    while(0);

    if (bError)
    {
        // todo: remove element and release ID
        SNP_BITFIELD_DATA_SET(oVertexID.m_asBitfield._raw, 0);
    }

    snp_leave_critical_section();
    return oVertexID.m_asAddr;
}

//////////////////////////////////////////////////////////////////////////////////
//// Private methods, no multithread guard is needed

void snp_enter_critical_section()
{
    // not implemented yet
}

void snp_leave_critical_section()
{
    // not implemented yet
}

tVertexID snp_vertex_id_obtain()
{
    union
    {
        tVertexID m_astVertexID;
        uint32 m_asUint32;
    } ID;

    static uint32 s_tVertexID = 0;
    ID.m_asUint32 = s_tVertexID++;

    return ID.m_astVertexID;
}

bool snp_vertex_create(const tVertexID &vertex_id, sc_type type, sc_access_levels access_levels)
{
    tCell oSearchMask;
    SNP_BITFIELD_DATA_SET(oSearchMask.m_asBitfield._raw, 0);
    oSearchMask.m_asVertex.m_uType = -1;

    tCell oSearchTag;
    oSearchTag.m_asVertex.m_uType = static_cast<uint8>(tCellType_EMPTY);

    tCell oWriteMask;
    SNP_BITFIELD_DATA_SET(oWriteMask.m_asBitfield._raw, ~0);

    tCell oWriteData;
    SNP_BITFIELD_DATA_SET(oWriteData.m_asBitfield._raw, 0);
    oWriteData.m_asVertex.m_uType = static_cast<uint8>(tCellType_VERTEX);
    oWriteData.m_asVertex.m_ID.m_asBitfield = vertex_id.m_asBitfield;
    oWriteData.m_asVertex.m_scType = type;
    oWriteData.m_asVertex.m_scAccessLevels = access_levels;

    tDevice::tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode oErrorCode;
    bool bCreated = s_oDevice.Exec(true, snp::tOperation_Assign, oInstruction, &oErrorCode);
    if (oErrorCode != CErrorCode::SUCCEEDED)
    {
        // print error log accordingly to returned code
        // for now just assert in any case
        assert(0);
        return false;
    }
    return bCreated;
}

bool snp_edge_create(const tVertexID &vertex_id1, const tVertexID &vertex_id2)
{
    tCell oSearchMask;
    SNP_BITFIELD_DATA_SET(oSearchMask.m_asBitfield._raw, 0);
    oSearchMask.m_asEdge.m_uType = 0x03;

    tCell oSearchTag;
    oSearchTag.m_asEdge.m_uType = static_cast<uint8>(tCellType_EMPTY);

    tCell oWriteMask;
    SNP_BITFIELD_DATA_SET(oWriteMask.m_asBitfield._raw, ~0);

    tCell oWriteData;
    SNP_BITFIELD_DATA_SET(oWriteData.m_asBitfield._raw, 0);
    oWriteData.m_asEdge.m_uType = static_cast<uint8>(tCellType_EDGE);
    oWriteData.m_asEdge.m_ID1.m_asBitfield = vertex_id1.m_asBitfield;
    oWriteData.m_asEdge.m_ID2.m_asBitfield = vertex_id2.m_asBitfield;

    tDevice::tInstruction oInstruction;
    oInstruction.m_oSearchMask = oSearchMask.m_asBitfield;
    oInstruction.m_oSearchTag = oSearchTag.m_asBitfield;
    oInstruction.m_oWriteMask = oWriteMask.m_asBitfield;
    oInstruction.m_oWriteData = oWriteData.m_asBitfield;

    CErrorCode oErrorCode;
    bool bCreated = s_oDevice.Exec(true, snp::tOperation_Assign, oInstruction, &oErrorCode);
    if (oErrorCode != CErrorCode::SUCCEEDED)
    {
        // print error log accordingly to returned code
        // for now just assert in any case
        assert(0);
        return false;
    }
    return bCreated;
}

#endif //ENABLE_HARDWARE_STORAGE
