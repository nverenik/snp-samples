///////////////////////////////////////////////////////////////////////
// File:	map_snp.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef __MAP_SNP_H__
#define __MAP_SNP_H__

#include "code_snp.h"
struct tmap_snp {
	tstructure_snp *  index;
	telement i;
	tmap_snp() {	i = 0; index = NULL;	}
	tmap_snp(unsigned s) {	i = 0; index = new tstructure_snp[s];	}
	~tmap_snp() {	if (index != NULL) delete[] index;	}
};

bool fmapped_snp(telement e,tmap_snp& m);
tmap_snp& fmap_snp(telement e,tstructure_snp s,tmap_snp& m);
tstructure_snp fget_snp(telement e,tmap_snp& m);
unsigned ffinalizer_snp(ttext&, tcode_snp&, tmap_snp&);
#endif//__MAP_SNP_H__
