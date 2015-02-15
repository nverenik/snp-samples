///////////////////////////////////////////////////////////////////////
// File:	base64.h
// Date:	9.12.2005
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _BASE64_H_
#define _BASE64_H_

#include "types.h"

unsigned char* encode_base64(unsigned char *src, int size, int *dest_sz);
unsigned char* decode_base64(unsigned char *src, int size, int *dest_sz);
unsigned char b64_2_byte(unsigned char b64_symbol);

int read_b64_file(const char *, int, BYTE **, int *);
int write_b64_file(const char *, const char *, const BYTE *, const int);

#endif // _BASE64_H_
