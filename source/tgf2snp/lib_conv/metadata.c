///////////////////////////////////////////////////////////////////////
// File:	metadata.c
// Date:	4.01.2011
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "base64.h"
#include "metadata.h"

const char metadata_header[] = ">>format_3.2_data<<";

//
int metadata_2_buffer(BYTE **dest_buf, int *dest_sz, METADATA *pmeta)
{
	BYTE *dest = 0;
	
	int i, j;
	int dest_size = 2*sizeof(int); /* number of prefixes + number of vertexes */
	
	for(i = 0; i < pmeta->prefix_num; i++)
	{
		dest_size += sizeof(int); /* idtf_size */
		dest_size += (pmeta->prefix_data[i].idtf_size)*sizeof(BYTE);
	}

	for(i = 0; i < pmeta->vertex_num; i++)
	{
		dest_size += 4*sizeof(int); /* idtf_size + data_type + data_size + prefix_id */
		dest_size += (pmeta->vertex_data[i].idtf_size)*sizeof(BYTE);
		dest_size += (pmeta->vertex_data[i].data_size)*sizeof(BYTE);
	}

	dest = (BYTE*)malloc(dest_size*sizeof(BYTE));

	if(!dest)
	{
		return MEMORY_ERROR;
	}

	*dest_buf = dest;

	memcpy(dest, &(pmeta->prefix_num), sizeof(int));
	memcpy(dest + sizeof(int), &(pmeta->vertex_num), sizeof(int));
	dest += 2*sizeof(int);

	for(i = 0; i < pmeta->prefix_num; i++)
	{
		memcpy(dest, &(pmeta->prefix_data[i].idtf_size),
			sizeof(int));
		dest += sizeof(int);
		
		for(j = 0; j < pmeta->prefix_data[i].idtf_size; j++)
		{
			*(dest++) = pmeta->prefix_data[i].idtf[j];
		}
	}

	for(i = 0; i < pmeta->vertex_num; i++)
	{
		memcpy(dest, &(pmeta->vertex_data[i].prefix_id), sizeof(int));
		memcpy(dest + sizeof(int), &(pmeta->vertex_data[i].idtf_size),
			sizeof(int));
		memcpy(dest + 2*sizeof(int), &(pmeta->vertex_data[i].data_size),
			sizeof(int));
		memcpy(dest + 3*sizeof(int), &(pmeta->vertex_data[i].data_type),
			sizeof(int));
		dest += 4*sizeof(int);

		for(j = 0; j < pmeta->vertex_data[i].idtf_size; j++)
		{
			*(dest++) = pmeta->vertex_data[i].idtf[j];
		}

		for(j = 0; j < pmeta->vertex_data[i].data_size; j++)
		{
			*(dest++) = pmeta->vertex_data[i].data[j]; //f3 dg6 float.fmt float.dg6 float.meta
		}												//dg6 f3 float.dg6 float1.f3 float.meta
	}

	*dest_sz = dest_size;
	
	return OK;
}

//
int buffer_2_metadata(BYTE *src_buf, int src_size, METADATA *pmeta)
{
	int i, j, k = 0;
	int rv;

	(void)src_size;

	if(!src_buf)
	{
		return OK;
	}

	memcpy(&(pmeta->prefix_num), src_buf + k, sizeof(int));
	k += sizeof(int);
	memcpy(&(pmeta->vertex_num), src_buf + k, sizeof(int));
	k += sizeof(int);

	rv = allocate_metadata(pmeta, pmeta->prefix_num, pmeta->vertex_num);

	if(rv != OK)
	{
		return rv;
	}

	for(i = 0; i < pmeta->prefix_num; i++)
	{
		pmeta->prefix_data[i].id = i;
		memcpy(&(pmeta->prefix_data[i].idtf_size), src_buf + k,
			sizeof(int));
		k += sizeof(int);

		pmeta->prefix_data[i].idtf = (BYTE*)malloc(pmeta->
			prefix_data[i].idtf_size*sizeof(BYTE));

		if(pmeta->prefix_data[i].idtf_size && (!pmeta->prefix_data[i].idtf))
		{
			return MEMORY_ERROR;
		}
		
		for(j = 0; j < pmeta->prefix_data[i].idtf_size; j++)
		{
			pmeta->prefix_data[i].idtf[j] = src_buf[k++];
		}
	}

	for(i = 0; i < pmeta->vertex_num; i++)
	{
		memcpy(&(pmeta->vertex_data[i].prefix_id), src_buf + k, sizeof(int));
		k += sizeof(int);
		memcpy(&(pmeta->vertex_data[i].idtf_size), src_buf + k,	sizeof(int));
		k += sizeof(int);
		memcpy(&(pmeta->vertex_data[i].data_size), src_buf + k,	sizeof(int));
		k += sizeof(int);
		memcpy(&(pmeta->vertex_data[i].data_type), src_buf + k,	sizeof(int));
		k += sizeof(int);

		pmeta->vertex_data[i].idtf = (BYTE*)malloc(pmeta->
			vertex_data[i].idtf_size*sizeof(BYTE));

		pmeta->vertex_data[i].data = (BYTE*)malloc(pmeta->
			vertex_data[i].data_size*sizeof(BYTE));

		if(pmeta->vertex_data[i].idtf_size && pmeta->vertex_data[i].data_size && 
			((!pmeta->vertex_data[i].idtf) || (!pmeta->vertex_data[i].data)))
		{
			return MEMORY_ERROR;
		}

		for(j = 0; j < pmeta->vertex_data[i].idtf_size; j++)
		{
			 pmeta->vertex_data[i].idtf[j] = src_buf[k++];
		}

		for(j = 0; j < pmeta->vertex_data[i].data_size; j++)
		{
			 pmeta->vertex_data[i].data[j] = src_buf[k++];
		}
	}

	return OK;
}

//
int write_metadata(const char *fname, BYTE *data, int data_size)
{
	int rv;
	int enc_size = 0;

	BYTE *encoded = encode_base64(data, data_size, &enc_size);

	if(!encoded)
	{
		return MEMORY_ERROR;
	}

	rv = write_b64_file(metadata_header, fname, encoded, enc_size);

	free(encoded);

	return rv;
}

//
int read_metadata(const char *fname, BYTE **data, int *data_size)
{
	int rv;
	int enc_size = 0;

	BYTE *encoded = 0;

	rv = read_b64_file(fname, (int)strlen(metadata_header), &encoded, &enc_size);

	if(rv != OK)
	{
		return rv;
	}

	*data = decode_base64(encoded, enc_size, data_size);

	free(encoded);

	if(!data)
	{
		return FAILED;
	}

	return OK;
}

//
int allocate_metadata(METADATA *pmeta, int prefix_cnt, int vertex_cnt)
{
	int i;

	if(prefix_cnt)
	{
		pmeta->prefix_data = (PREFIX*)malloc(prefix_cnt*sizeof(PREFIX));

		if(!pmeta->prefix_data)
		{
			return MEMORY_ERROR;
		}
	}
	else
	{
		pmeta->prefix_data = 0;
	}

	if(vertex_cnt)
	{
		pmeta->vertex_data = (DATA*)malloc(vertex_cnt*sizeof(DATA));

		if(!pmeta->vertex_data)
		{
			free(pmeta->prefix_data);
			return MEMORY_ERROR;
		}
	}

	pmeta->prefix_num = prefix_cnt;
	pmeta->vertex_num = vertex_cnt;

	for(i = 0; i < prefix_cnt; i++)
	{
		pmeta->prefix_data[i].idtf_size = 0;
	}

	for(i = 0; i < vertex_cnt; i++)
	{
		pmeta->vertex_data[i].prefix_id = -1;
		pmeta->vertex_data[i].data_type = ARG_UNDEF;
		pmeta->vertex_data[i].data_size = 0;
		pmeta->vertex_data[i].idtf_size = 0;
	}

	return OK;
}

//
void delete_metadata(METADATA *pmeta)
{
	int i;

	for(i = 0; i < pmeta->prefix_num; i++)
	{
		if(pmeta->prefix_data[i].idtf)
		{
			free(pmeta->prefix_data[i].idtf);
		}
	}

	if(pmeta->prefix_num != 0)
	{
		free(pmeta->prefix_data);
	}

	for(i = 0; i < pmeta->vertex_num; i++)
	{
		if(pmeta->vertex_data[i].idtf_size)
		{
			free(pmeta->vertex_data[i].idtf);
		}

		if(pmeta->vertex_data[i].data_size)
		{
			free(pmeta->vertex_data[i].data);
		}
	}

	if(pmeta->vertex_num != 0)
	{
		free(pmeta->vertex_data);
	}

	pmeta->prefix_num = pmeta->vertex_num = 0;
}
