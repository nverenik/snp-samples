///////////////////////////////////////////////////////////////////////
// File:	code_snp.cpp
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include "code_snp.h"

sc_type encode(ttype t) {
	sc_type r = (sc_type_const&sc_type_var);
	ttype META = element_type(1<<F_LINK);
	ttype VAR = element_type(1<<F_AREA);
#ifdef  CONST
#undef  CONST
#endif//CONST
	ttype CONST = element_type(1<<F_SEGMENT);
	r = (((t & NODE)	== NODE)?	(r|sc_type_node): r);
	r = (((t & (APP|CONST))==(APP|CONST))?(r|sc_type_arc_access): r);
	r = (((t & ARC)		== ARC)?	(((t & (APP|CONST)) == (APP|CONST))? r: (r|sc_type_arc_common)): r);
	r = (((t & CONST)	== CONST)?	(r|sc_type_const): r);
	r = (((t & VAR)		== VAR)?	(r|sc_type_var): r);
	r = (((t & META)	== META)?	(r|sc_type_var): r);
	r = (((t & POSITIVE)== POSITIVE)?(r|sc_type_arc_pos): r);
	r = (((t & NEGATIVE)== NEGATIVE)?(r|sc_type_arc_neg): r);
	r = (((t & FUZZY)	== FUZZY)?	(r|sc_type_arc_fuz): r);
	r = (((t & PERMANENT)==PERMANENT)?(r|sc_type_arc_perm): r);
	r = (((t & TEMPORARY)==TEMPORARY)?(r|sc_type_arc_temp): r);
	return r;
};

void initialize_snp() {
    snp_initialize(NULL, true);
}
void destroy_snp() {
    snp_shutdown(false);
}
tstructure_snp fgen_snp(tcode_snp& c,telement e,ttype t) {
	c.o = tcode_snp::GEN;
	c.i = e;
	c.t = t;
	return tstructure_snp(snp_element_create_node(encode(t), *c.paccess_levels));
};
tstructure_snp fgen_beg_snp(tcode_snp& c,telement e,ttype t,tstructure_snp b) {
	c.o = tcode_snp::GEN_BEG;
	c.i = e;
	c.b = b;
	c.t = t;
	return tstructure_snp(snp_element_create_arc_begin(encode(t), b, *c.paccess_levels));
};
tstructure_snp fgen_beg_end_snp(tcode_snp& c,telement i,ttype t,tstructure_snp b,tstructure_snp e) {
	c.o = tcode_snp::GEN_BEG_END;
	c.i = i;
	c.b = b;
	c.e = e;
	c.t = t;
	return tstructure_snp(snp_element_create_arc(encode(t), b, e,*c.paccess_levels));
};
tstructure_snp fgen_loop_snp(tcode_snp& c,telement i,ttype t,tstructure_snp b,telement) {
	c.o = tcode_snp::GEN_LOOP;
	c.i = i;
	c.e = snp_element_create_arc_loop(encode(t), b, *c.paccess_levels);
	c.b = b;
	c.t = t;
	return c.e;
};
tstructure_snp fset_end_snp(tcode_snp& c,tstructure_snp s,tstructure_snp e) {
	c.o = tcode_snp::NOP;
	return tstructure_snp(snp_element_set_arc_end(s, e, *c.paccess_levels));
};
tstructure_snp fidentify_snp(tcode_snp&,tstructure_snp s,tidentifier) {	return s;	};
tstructure_snp fcontain_snp(tcode_snp& c,tstructure_snp s,tcontent) {
	unsigned k = 0, unit_size = c.unit_size;
	telement *  index = c.ptext->index;
	METADATA& metadata = c.ptext->metadata;
	telement i = c.i;
	c.o = tcode_snp::NOP;
	return s;
};