///////////////////////////////////////////////////////////////////////
// File:	metadata.h
// Date:	11.12.2005
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _METADATA_H_
#define _METADATA_H_

#include "types.h"

int metadata_2_buffer(BYTE **, int *, METADATA *);
int buffer_2_metadata(BYTE *, int, METADATA *);
int write_metadata(const char *, BYTE *, int);
int read_metadata(const char *, BYTE **, int *);
int allocate_metadata(METADATA *, int, int);
void delete_metadata(METADATA *);

#endif //_METADATA_H_
