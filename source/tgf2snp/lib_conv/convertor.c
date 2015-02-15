///////////////////////////////////////////////////////////////////////
// File:	convertor.c
// Date:	7.03.2009
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include "types.h"
#include "convertor.h"
#include "common.h"
#include "matrix.h"
#include "metadata.h"
#include "format3.h"

#include <string.h>
#include <stdlib.h>

//
int f3_2_adj_matrix(char *fname, BYTE ***matrix, METADATA *pmeta)
{
	int rv, fsize;
	BYTE beg_pos;

	BYTE *buf = 0;
	F3_HEADER header;

	rv = read_file(fname, "rb", &buf);

	if(rv < 0)
	{
		return rv;
	}

	fsize = rv;

	parse_f3_header(buf, &header);
	beg_pos = (BYTE)ADJUST(header.size);

	rv = check_f3_crc(buf, beg_pos, fsize, pmeta);

	if(rv != OK)
	{
		free(buf);
		return rv;
	}

	rv = allocate_matrix(matrix, pmeta);

	if(rv != OK)
	{
		free(buf);
		return rv;
	}

	rv = parse_f3_crc(buf, *matrix, beg_pos, fsize, pmeta);

	if(rv != OK)
	{
		free(buf);
		return rv;
	}

	return OK;
}

//
int adj_matrix_2_f3(char *fname, BYTE **matrix, METADATA *pmeta, UINT *  index)
{
	F3_HEADER header;
	const char descript[] = "description";

	header.signature[0] = 'T';
	header.signature[1] = 'G';
	header.signature[2] = 'F';
	header.minor_version = 2;
	header.major_version = 3;
	header.endianness = 1;
	header.compression = 0;
	header.checksumming = 1;
	strcpy(header.description, descript);
	header.descr_size = (unsigned char)strlen(descript);
	header.size = sizeof(F3_HEADER);

	return write_f3(fname, &header, matrix, pmeta, index);
}