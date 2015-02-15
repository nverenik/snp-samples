///////////////////////////////////////////////////////////////////////
// File:	code_snp.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef __CODE_SNP_H__
#define __CODE_SNP_H__

#include "sctext.h"

#define ENABLE_HARDWARE_STORAGE
#define PURE_IMPLEMENTATION
#include "sc-memory/src/sc-store/sc_storage_snp/sc_storage_snp_glue.pure.h"

typedef sc_addr tstructure_snp;

struct tcode_snp {
	bool h,r,f,c,z;
	unsigned unit_size;
	enum tgen {
		NOP = 0,
		GEN = 1,
		GEN_BEG,
		GEN_LOOP,
		GEN_BEG_END
	} o;
	tstructure_snp b,e;
	telement i;
	ttype t;
	ttext *  ptext;
	sc_access_levels *  paccess_levels;
	tcode_snp() {
		ptext = NULL;
		h = r = f = c = z = false;
		unit_size = 0;
		o = NOP;
	};
	tcode_snp(ttext& t, sc_access_levels& a) {
		ptext = &t;
		o = NOP;
		paccess_levels = &a;
	};
	tcode_snp(ttext& t, bool h, bool r, bool f, bool c, bool z, unsigned u, sc_access_levels& a) {
		ptext = &t;
		this->h = h;
		this->r = r;
		this->f = f;
		this->c = c;
		this->z = z;
		unit_size = u;
		o = NOP;
		paccess_levels = &a;
	};
};

tstructure_snp fgen_snp(tcode_snp& c,telement e,ttype t);
tstructure_snp fgen_beg_snp(tcode_snp& c,telement e,ttype t,tstructure_snp b);
tstructure_snp fgen_beg_end_snp(tcode_snp& c,telement i,ttype t,tstructure_snp b,tstructure_snp e);
tstructure_snp fgen_loop_snp(tcode_snp& c,telement i,ttype t,tstructure_snp b,telement);
tstructure_snp fset_end_snp(tcode_snp& c,tstructure_snp s,tstructure_snp e);
tstructure_snp fidentify_snp(tcode_snp&,tstructure_snp s,tidentifier);
tstructure_snp fcontain_snp(tcode_snp& c,tstructure_snp s,tcontent);
#endif//__CODE_SNP_H__