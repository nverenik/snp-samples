///////////////////////////////////////////////////////////////////////
// File:	sctranslator_snp.cpp
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include "sctranslator_snp.h"

unsigned translate_snp(BYTE ** adj, METADATA metadata, sc_access_levels access_levels) {
	ttext t(adj,metadata);
	tcode_snp c(t,access_levels);
	tmap_snp m(t.size);
	return sctranslate <	ttext&,	ttextselector,	telement, ttype, tidentifier, tcontent,
					tcode_snp&,	tstructure_snp, tmap_snp&, 
					unsigned,
					fedges,
					fidentified,
					fcontained,
					ftype_constancy,
					fidentifier,
					fcontent,
					fbeg,
					fend,
					fgen_snp,
					fset_end_snp,
					fidentify_snp,
					fcontain_snp,
					fgen_beg_snp,
					fgen_beg_end_snp,	//gen_beg_end	<telement, ttype, tcode, tstructure, fset_end_snp, fgen_beg_snp>,
					fgen_loop_snp,		//gen_loop		<telement, ttype, tcode, tstructure, fset_end_snp, fgen_beg_snp>,
					fmapped_snp,
					fget_snp,
					fmap_snp,
					ffinalizer_snp>(t,c,m);
};
unsigned translate_snp(char *  file, sc_access_levels access_levels) {
 	BYTE ** adj;
	METADATA metadata;
	unsigned r = ~0;
	if (f3_2_adj_matrix(file, &adj, &metadata) == OK) {
		r = translate_snp(adj,metadata,access_levels);
		delete_matrix(&adj,&metadata);
		delete_metadata(&metadata);
	}
	return r;
};
