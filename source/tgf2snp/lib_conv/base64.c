///////////////////////////////////////////////////////////////////////
// File:	base64.c
// Date:	14.12.2005
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include <stdlib.h>
#include <stdio.h>

#include "base64.h"
#include "types.h"


const BYTE B64_ALPHABET[] = 
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U',
	'V', 'W', 'X', 'Y', 'Z',

	'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u',
	'v', 'w', 'x', 'y', 'z',

	'0', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '+', '/'
};

//
BYTE* encode_base64(BYTE *src, int size, int *dest_sz)
{
	BYTE *dest = 0;

	int i, j;
	int oct_data = 0;
	int dest_size;
	int bits_num = size*8;
	int oct_cnt = bits_num/6 + ((bits_num % 6) ? (1) : (0));

	oct_cnt += ((oct_cnt % 4) ? (4 - (oct_cnt % 4)) : (0));
	dest_size = oct_cnt;

	dest = (BYTE*)malloc(dest_size*sizeof(BYTE));

	if(!dest)
	{
		return 0;
	}

	for(i = 0, j = 0; i < size/3; i++)
	{
		oct_data = (src[i*3] & 252) >> 2;
		dest[j++] = B64_ALPHABET[oct_data];
 
		oct_data = ((src[i*3] & 3) << 4) + ((src[i*3+1] & 240) >> 4);
		dest[j++] = B64_ALPHABET[oct_data];

		oct_data = ((src[i*3+1] & 15) << 2) + ((src[i*3+2] & 192) >> 6);
		dest[j++] = B64_ALPHABET[oct_data];

		oct_data = (src[i*3+2] & 63);
		dest[j++] = B64_ALPHABET[oct_data];
	}

	if(size % 3 == 1)
	{
		oct_data = (src[size-1] & 252) >> 2;
		dest[j++] = B64_ALPHABET[oct_data];

		oct_data = ((src[size-1] & 3) << 4);
		dest[j++] = B64_ALPHABET[oct_data];

		dest[j++] = '=';
		dest[j++] = '=';
	}
	else if(size % 3 == 2)
	{
		oct_data = (src[size-2] & 252) >> 2;
		dest[j++] = B64_ALPHABET[oct_data];
 
		oct_data = ((src[size-2] & 3) << 4) + ((src[size-1] & 240) >> 4);
		dest[j++] = B64_ALPHABET[oct_data];

		oct_data = ((src[size-1] & 15) << 2);
		dest[j++] = B64_ALPHABET[oct_data];

		dest[j++] = '=';
	}

	*dest_sz = dest_size;

	return dest;
}

//
BYTE* decode_base64(BYTE *src, int size, int *dest_sz)
{
	int i, j, oct_cnt = 0;
	BYTE *dest = 0;

	int dest_size;
	int iter = 0;

	dest_size = (size/4)*3;

	if(src[size-1] == '=')
	{
		dest_size -= 1;
	}
	if(src[size-2] == '=')
	{
		dest_size -= 1;
	}

	dest = (BYTE*)malloc(dest_size*sizeof(BYTE));

	if(!dest)
	{
		return 0;
	}

	i = 0;
	j = 0;
	oct_cnt = 0;
	iter = dest_size - (dest_size % 3);

	while(i < iter)
	{
		dest[i++] = (b64_2_byte(src[j++]) << 2) + 
			((b64_2_byte(src[j]) & 48) >> 4);

		dest[i++] = ((b64_2_byte(src[j++]) & 15) << 4) +
			((b64_2_byte(src[j]) & 60) >> 2);

		dest[i++] = ((b64_2_byte(src[j++]) & 3) << 6) +
			b64_2_byte(src[j++]);
	}

	if(dest_size % 3 == 1)
	{
		dest[i++] = (b64_2_byte(src[j++]) << 2) + 
			((b64_2_byte(src[j]) & 48) >> 4);
	}
	else if(dest_size % 3 == 2)
	{
		dest[i++] = (b64_2_byte(src[j++]) << 2) + 
			((b64_2_byte(src[j]) & 48) >> 4);

		dest[i++] = ((b64_2_byte(src[j++]) & 15) << 4) +
			((b64_2_byte(src[j]) & 60) >> 2);
	}

	*dest_sz = dest_size;

	return dest;
}

//
BYTE b64_2_byte(BYTE b64_symbol)
{
	BYTE i = 0;

	if((b64_symbol >= 48) && (b64_symbol <= 57))
	{
		for(i = 52; i <= 61; i++)
		{
			if(B64_ALPHABET[i] == b64_symbol)
			{
				return i;
			}
		}
	}
	else if((b64_symbol >= 65) && (b64_symbol <= 90))
	{
		for(i = 0; i <= 25; i++)
		{
			if(B64_ALPHABET[i] == b64_symbol)
			{
				return i;
			}
		}
	}
	else if((b64_symbol >= 97) && (b64_symbol <= 122))
	{
		for(i = 26; i <= 51; i++)
		{
			if(B64_ALPHABET[i] == b64_symbol)
			{
				return i;
			}
		}
	}
	else if(b64_symbol == 43)
	{
		return 62;
	}

	return 63;
}

//
int read_b64_file(const char *fname, int header_size,
				  BYTE **data, int *data_size)
{
	int i;
	FILE *in = fopen(fname, "rb");

	if(!in)
	{
		printf("\nCan't read file %s\n", fname);
		return FILE_ERROR;
	}

	fseek(in, 0, SEEK_END);
	*data_size = ftell(in) - header_size - 2;
	fseek(in, header_size + 2, SEEK_SET);

	*data_size -= ((*data_size)/78)*2;

	*data = (BYTE*)malloc((*data_size)*sizeof(BYTE));

	if(!(*data))
	{
		return MEMORY_ERROR;
	}

	for(i = 0; i < (*data_size)/76; i++)
	{
		fread(*data + 76*i*sizeof(BYTE), 1, 76*sizeof(BYTE), in);
		fseek(in, 2, SEEK_CUR);
	}
	
	fread(*data + 76*((*data_size)/76)*sizeof(BYTE), 1,
		((*data_size) % 76), in);

	fclose(in);

	return OK;
}

//
int write_b64_file(const char *header, const char *fname,
				   const BYTE *data, const int data_size)
{
	int i;
	FILE *out = fopen(fname, "wb");

	if(!out)
	{
		return FILE_ERROR;
	}

	fprintf(out, "%s\r\n", header);

	for(i = 0; i < data_size/76; i++)
	{
		fwrite(data + 76*i*sizeof(BYTE), 1, 76, out);
		fprintf(out, "\r\n");
	}
	
	fwrite(data + 76*(data_size/76)*sizeof(BYTE), 1,
		(data_size % 76), out);

	fclose(out);

	return OK;
}
