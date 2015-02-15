///////////////////////////////////////////////////////////////////////
// File:	convertor.h
// Date:	21.12.2005
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _CONVERTOR_H_
#define _CONVERTOR_H_

#include "types.h"

int f3_2_adj_matrix(char *, BYTE ***, METADATA *);
int adj_matrix_2_f3(char *, BYTE **, METADATA *, UINT *  index);

#endif //_CONVERTOR_H_
