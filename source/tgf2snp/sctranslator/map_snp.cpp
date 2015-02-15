///////////////////////////////////////////////////////////////////////
// File:	map_snp.cpp
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include "map_snp.h"

bool fmapped_snp(telement e,tmap_snp& m) { return m.i > e; };
tmap_snp& fmap_snp(telement e,tstructure_snp s,tmap_snp& m) {	m.index[(m.i = e)++] = s;	return m;	};
tstructure_snp fget_snp(telement e,tmap_snp& m) {	return m.index[e];	};
unsigned ffinalizer_snp(ttext&, tcode_snp&, tmap_snp&) {	return 0;	};