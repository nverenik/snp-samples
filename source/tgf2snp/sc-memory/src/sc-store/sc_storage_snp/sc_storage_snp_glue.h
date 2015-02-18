///////////////////////////////////////////////////////////////////////
// File:	sc_storage_snp_glue.pure.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	semantic network processor pure implementation

#ifndef _sc_storage_snp_glue_pure_h_
#define _sc_storage_snp_glue_pure_h_

#ifdef ENABLE_HARDWARE_STORAGE

#ifdef __cplusplus
extern "C" {
#endif

#include "../sc_types.h"
#include "../sc_stream.h"

sc_bool     snp_initialize(const char *path, sc_bool clear);
void        snp_shutdown(sc_bool save_state);
sc_bool     snp_is_initialized();

sc_addr     snp_element_create_node(sc_type type, sc_access_levels access_levels);
sc_addr     snp_element_create_arc(sc_type type, sc_addr beg, sc_addr end, sc_access_levels access_levels);
sc_addr     snp_element_create_arc_begin(sc_type type, sc_addr beg, sc_access_levels access_levels);
sc_addr     snp_element_create_arc_loop(sc_type type, sc_addr beg, sc_access_levels access_levels);

sc_addr     snp_element_set_arc_end(sc_addr addr, sc_addr end, sc_access_levels access_levels);

#ifdef __cplusplus
}
#endif

#endif //ENABLE_HARDWARE_STORAGE

#endif //_sc_storage_snp_glue_pure_h_
