///////////////////////////////////////////////////////////////////////
// File:	common.h
// Date:	12.12.2005
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _COMMON_H_
#define _COMMON_H_

#include "types.h"

#define MIN(a, b)	(((a) < (b)) ? (a) : (b))

int read_file(char *, char *, BYTE **);
BYTE* encode_N(int);
int decode_N(BYTE *);

#endif //_COMMON_H_
