///////////////////////////////////////////////////////////////////////
// File:	format3.h
// Date:	7.03.2009
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _FORMAT3_H_
#define _FORMAT3_H_

#include "types.h"
#include <stdio.h>

#define ADJUST(x)	((((x) % 4) == 0) ? (x) : (((x)/4)*4 + 4))

// functions

void parse_f3_header(BYTE *, F3_HEADER *);
int check_f3_crc(BYTE *, BYTE, long, METADATA *);
int parse_f3_crc(BYTE *, BYTE **, WORD, long, METADATA *);
void set_vertex_type(BYTE, int, BYTE **);
void get_vertex_type(BYTE *, int, BYTE **);
int write_f3(const char *, F3_HEADER *, BYTE **, METADATA *, UINT *  index);
void adjust_file(FILE *);
int write_f3_file_crc(FILE *, BYTE **, METADATA *, UINT *  index);

#endif //_FORMAT3_H_
