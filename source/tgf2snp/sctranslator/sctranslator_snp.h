///////////////////////////////////////////////////////////////////////
// File:	sctranslator_snp.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef __SCTRANSLATOR_SNP_H__
#define __SCTRANSLATOR_SNP_H__

#include "map_snp.h"
#include "sctranslator.h"

unsigned translate_snp(BYTE ** adj, METADATA metadata, sc_access_levels access_levels);
unsigned translate_snp(char *  file, sc_access_levels access_levels);
#endif//__SCTRANSLATOR_SNP_H__


