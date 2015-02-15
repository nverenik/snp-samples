///////////////////////////////////////////////////////////////////////
// File:	common.c
// Date:	7.03.2009
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "common.h"

//
int read_file(char *fname, char *mode, BYTE **buf)
{
	FILE *f;
	long fsize = 0;

	f = fopen(fname, mode);

	if(!f)
	{
		printf("\nCan't read file %s\n", fname);
		return FILE_ERROR;
	}

	//get file size
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	*buf = (BYTE*)malloc(fsize+1);

	if(!(*buf))
	{
		fclose(f);
		return MEMORY_ERROR;
	}

	fread(*buf, 1, fsize*sizeof(BYTE), f);
	(*buf)[fsize] = 0;

	fclose(f);

	return fsize;
}

//
BYTE* encode_N(int x)
{
	BYTE *ptr = 0;
	int byte1, byte2, byte3;

	if(x < 63)
	{
		ptr = (BYTE*)malloc(2*sizeof(BYTE));

		if(ptr)
		{
			sprintf((char*)ptr, "%c", x + 63);
		}
	}
	else
	{
		ptr = (BYTE*)malloc(5*sizeof(BYTE));

		if(ptr)
		{
			_asm
			{
				mov		eax,	x
				mov		ebx,	eax
				mov		ecx,	eax
				
				and		eax,	63

				shr		ebx,	6
				and		ebx,	63
				
				shr		ecx,	12
				and		ecx,	63

				add		eax,	63
				add		ebx,	63
				add		ecx,	63

				mov		byte1,	ecx
				mov		byte2,	ebx
				mov		byte3,	eax
			}

			sprintf((char*)ptr, "%c%c%c%c", 126, byte1, byte2, byte3);
		}
	}

	return ptr;
}

//
int decode_N(BYTE *src)
{
	int byte1, byte2, byte3;
	int res = 0;

	if(src[0] < 126)
	{
		return (int)(src[0] - 63);
	}

	byte1 = src[1];
	byte2 = src[2];
	byte3 = src[3];

	_asm
	{
		mov		eax,	byte1
		mov		ebx,	byte2
		mov		ecx,	byte3

		sub		eax,	63
		sub		ebx,	63
		sub		ecx,	63

		shl		eax,	12
		shl		ebx,	6

		add		eax,	ebx
		add		eax,	ecx

		mov		res,	eax
	}

	return res;
}
