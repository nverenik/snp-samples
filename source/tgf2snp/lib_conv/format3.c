///////////////////////////////////////////////////////////////////////
// File:	format3.c
// Date:	9.12.2011
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"
#include "types.h"
#include "format3.h"
#include "common.h"
#include "metadata.h"

//parse format3 file header
void parse_f3_header(BYTE *buf, F3_HEADER *header)
{
	header->signature[0] = buf[0];
	header->signature[1] = buf[1];
	header->signature[2] = buf[2];

	header->minor_version = buf[3];
	header->major_version = buf[4];
	header->endianness = buf[5];

	header->compression = buf[6];
	header->compression += (buf[7] >> 8);

	header->checksumming = buf[9];

	header->descr_size = buf[12];

	memcpy(header->description, buf+13, buf[12]);
	header->description[buf[12]] = 0;

	header->size = 13 + buf[12];
}

//
int check_f3_crc(BYTE *buf, BYTE beg_pos, long fsize, METADATA *pmeta)
{
	WORD cmd_type = 0;
	WORD args_num = 0;
	int i, j, index = beg_pos;
	int arg_type = 0;
	int data32 = 0;
	int cmd_count = 0;
	int vertex_count = 0;
	int prefix_count = 0;
	BYTE checksum = 0;
	int rv = OK;

	while(1)
	{
		checksum = 0;

		checksum ^= buf[index];
		cmd_type = buf[index++];
		checksum ^= buf[index];
		cmd_type += (buf[index++] << 8);

		if((cmd_type == CMD_GEN) || (cmd_type == CMD_FIND_BY_IDTF))
		{
			vertex_count++;
		}
		if(cmd_type == CMD_DECLARE_PREFIX)
		{
			prefix_count++;
		}

		if((cmd_type != CMD_NOP) && (cmd_type < CMD_END_OF_STREAM))
		{
			checksum ^= buf[index];
			args_num = buf[index++];
			checksum ^= buf[index];
			args_num += (buf[index++] << 8);
		}
		else
		{
			args_num = 0;
			index += 2;
		}

		for(i = 0; i < args_num; i++)
		{
			checksum ^= buf[index];
			arg_type = buf[index++];
			checksum ^= buf[index];
			arg_type += (buf[index++] << 8);
			checksum ^= buf[index];
			arg_type += (buf[index++] << 16);
			checksum ^= buf[index];
			arg_type += (buf[index++] << 24);

			switch(arg_type)
			{
				case ARG_INT32:
					for(j = 0; j < 4; j++)
					{
						checksum ^= buf[index++];
					}
					break;

				case ARG_INT64:
					for(j = 0; j < 8; j++)
					{
						checksum ^= buf[index++];
					}
					break;

				case ARG_FLOAT:
					for(j = 0; j < 8; j++)
					{
						checksum ^= buf[index++];
					}
					break;

				case ARG_DATA:
					checksum ^= buf[index];
					data32 = buf[index++];
					checksum ^= buf[index];
					data32 += (buf[index++] << 8);
					checksum ^= buf[index];
					data32 += (buf[index++] << 16);
					checksum ^= buf[index];
					data32 += (buf[index++] << 24);
					
					for(j = 0; j < data32; j++)
					{
						checksum ^= buf[index++];
					}

					if(i != (args_num-1))
					{
						index = ADJUST(index);
					}
					break;

				case ARG_TYPE:
					checksum ^= buf[index++];

					if(i != (args_num-1))
					{
						index += 3;
					}
					break;

				case ARG_INT16:
					checksum ^= buf[index++];
					checksum ^= buf[index++];

					if(i != (args_num-1))
					{
						index += 2;
					}
					break;

				case ARG_STRING:
					checksum ^= buf[index];
					data32 = buf[index++];
					checksum ^= buf[index];
					data32 += (buf[index++] << 8);
					checksum ^= buf[index];
					data32 += (buf[index++] << 16);
					checksum ^= buf[index];
					data32 += (buf[index++] << 24);
					
					for(j = 0; j < data32; j++)
					{
						checksum ^= buf[index++];
					}

					if(i != (args_num-1))
					{
						index = ADJUST(index);
					}
					break;

				default:
					break;
			}

			if(index > fsize)
			{
				return UNDEF_ERROR;
			}
			if(index == fsize)
			{
				break;
			}
		}

		//checksum
		if(checksum != buf[index++])
		{
			printf("\nWARNING: Command %d checksum NOT OK (offset 0x%x)\n", 
				cmd_count, index-1);

			rv = CRC_NOT_OK;
		}

		if(index == fsize)
		{
			break;
		}
		
		index = ADJUST(index);

		if(index > fsize)
		{
			return UNDEF_ERROR;
		}

		cmd_count++;
	}

	if(allocate_metadata(pmeta, prefix_count, vertex_count) == MEMORY_ERROR)
	{
		return MEMORY_ERROR;
	}

	return rv;
}

//
int find_vertex_index(int id, int *vert_arr, METADATA *pmeta)
{
	int i;

	for(i = MIN(pmeta->vertex_num, id); i >= 0; i--)
	{
		if(vert_arr[i] == id)
		{
			return i;
		}
	}

	return FAILED;
}

//
int parse_f3_crc(BYTE *buf, BYTE **matrix, WORD beg_pos, long fsize,
			 METADATA *pmeta)
{
	WORD cmd_type = 0;
	WORD args_num = 0;
	int i, j, index = beg_pos;
	int arg_type = 0;
	int data32 = 0;
	int cmd_count = 0;
	int vertex_count = 0;
	int prefix_count = 0;
	int current_prefix_id = EMPTY_PREFIX;
	int buffer = 0;
	BYTE byte = 0;

	int *vertex_array = 0;
		
	vertex_array = (int*)malloc(pmeta->vertex_num*sizeof(int));

	if(!vertex_array)
	{
		return MEMORY_ERROR;
	}

	while(1)
	{
		cmd_type = buf[index++];
		cmd_type += (buf[index++] << 8);

		switch(cmd_type)
		{
			case CMD_GEN:
			case CMD_FIND_BY_IDTF:
				vertex_array[vertex_count] = cmd_count;
				pmeta->vertex_data[vertex_count].prefix_id =
					current_prefix_id;
				args_num = buf[index++];
				args_num += (buf[index++] << 8);
				break;

			case CMD_DECLARE_PREFIX:
				pmeta->prefix_data[prefix_count].id = cmd_count;
				current_prefix_id = prefix_count;
				args_num = buf[index++];
				args_num += (buf[index++] << 8);
				break;

			case CMD_SWITCH_PREFIX:
				args_num = buf[index++];
				args_num += (buf[index++] << 8);
				break;

			case CMD_SET_BEG:
				args_num = buf[index++];
				args_num += (buf[index++] << 8);
				break;

			case CMD_SET_END:
				args_num = buf[index++];
				args_num += (buf[index++] << 8);
				break;

			default:
				args_num = 0;
				index += 2;
				break;
		}
		
		for(i = 0; i < args_num; i++)
		{
			arg_type = buf[index++];
			arg_type += (buf[index++] << 8);
			arg_type += (buf[index++] << 16);
			arg_type += (buf[index++] << 24);

			switch(arg_type)
			{
				case ARG_INT32:
					data32 = buf[index++];
					data32 += (buf[index++] << 8);
					data32 += (buf[index++] << 16);
					data32 += (buf[index++] << 24);

					if((cmd_type == CMD_SWITCH_PREFIX) && (i == 0 /* 1-st arg */))
					{
						if(data32 == -1)
						{
							current_prefix_id = -1;
						}
						else
						{
							for(j = MIN(data32, pmeta->prefix_num-1); j >= 0; j--)
							{
								if(pmeta->prefix_data[j].id == data32)
								{
									current_prefix_id = j;
									break;
								}
							}
						}
					}
					else if((cmd_type == CMD_SET_BEG) && (i == 0 /* 1-st arg */))
					{
						buffer = find_vertex_index(data32, vertex_array, pmeta);
					}
					else if((cmd_type == CMD_SET_BEG) && (i == 1 /* 2-nd arg */))
					{
						j = find_vertex_index(data32, vertex_array, pmeta);

						if((buffer != FAILED) && (j != FAILED))
						{
							SET_ELEMENT(matrix, buffer, j, 1);
							SET_ELEMENT(matrix, j, buffer, 1);
						}
					}
					else if((cmd_type == CMD_SET_END) && (i == 0 /* 1-st arg */))
					{
						buffer = find_vertex_index(data32, vertex_array, pmeta);
					}
					else if((cmd_type == CMD_SET_END) && (i == 1 /* 2-nd arg */))
					{
						j = find_vertex_index(data32, vertex_array, pmeta);

						if((buffer != FAILED) && (j != FAILED))
						{
							SET_ELEMENT(matrix, buffer, j, 1);
						}
					}
					else if((cmd_type == CMD_GEN) && (i == 2 /* 3-d argument */))
					{
						data32 = buf[index++];
						data32 += (buf[index++] << 8);
						data32 += (buf[index++] << 16);
						data32 += (buf[index++] << 24);

						pmeta->vertex_data[vertex_count].data_type = ARG_INT32;
						pmeta->vertex_data[vertex_count].data_size = sizeof(int);

						pmeta->vertex_data[vertex_count].data = 
							(BYTE*)malloc(sizeof(int));

						if(pmeta->vertex_data[vertex_count].data == 0)
						{
							free(vertex_array);
							return MEMORY_ERROR;
						}

						memcpy(&(pmeta->vertex_data[vertex_count].data), &data32,
							sizeof(int));
					}
					break;
				case ARG_INT64:
					index += 8; // Not supported
					break;
				case ARG_FLOAT:
					if((cmd_type == CMD_GEN) && (i == 2 /* 3-d argument */))
					{
						pmeta->vertex_data[vertex_count].data_type = ARG_FLOAT;
						pmeta->vertex_data[vertex_count].data_size = sizeof(double);

						pmeta->vertex_data[vertex_count].data = 
							(BYTE*)malloc(sizeof(double));

						if(pmeta->vertex_data[vertex_count].data == 0)
						{
							free(vertex_array);
							return MEMORY_ERROR;
						}

						pmeta->vertex_data[vertex_count].data[0] = buf[index++];
						pmeta->vertex_data[vertex_count].data[1] = buf[index++];
						pmeta->vertex_data[vertex_count].data[2] = buf[index++];
						pmeta->vertex_data[vertex_count].data[3] = buf[index++];
						pmeta->vertex_data[vertex_count].data[4] = buf[index++];
						pmeta->vertex_data[vertex_count].data[5] = buf[index++];
						pmeta->vertex_data[vertex_count].data[6] = buf[index++];
						pmeta->vertex_data[vertex_count].data[7] = buf[index++];
/*						pmeta->vertex_data[vertex_count].data[1] = ((buf[index++] << 8) >> 8);
						pmeta->vertex_data[vertex_count].data[2] = ((buf[index++] << 16) >> 16);
						pmeta->vertex_data[vertex_count].data[3] = ((buf[index++] << 24) >> 24);
						pmeta->vertex_data[vertex_count].data[4] = ((buf[index++] << 32) >> 32);
						pmeta->vertex_data[vertex_count].data[5] = ((buf[index++] << 40) >> 40);
						pmeta->vertex_data[vertex_count].data[6] = ((buf[index++] << 48) >> 48);
						pmeta->vertex_data[vertex_count].data[7] = ((buf[index++] << 56) >> 56);
*/					}
					break;
				case ARG_DATA:
					data32 = buf[index++];
					data32 += (buf[index++] << 8);
					data32 += (buf[index++] << 16);
					data32 += (buf[index++] << 24);

					if(cmd_type == CMD_GEN && i == 2 /* data */)
					{
						pmeta->vertex_data[vertex_count].data_size = data32;

						if(data32)
						{
							pmeta->vertex_data[vertex_count].data_type = ARG_DATA;

							pmeta->vertex_data[vertex_count].data = 
								(BYTE*)malloc(data32*sizeof(BYTE));

							if(pmeta->vertex_data[vertex_count].data == 0)
							{
								free(vertex_array);

								return MEMORY_ERROR;
							}

							memcpy(pmeta->vertex_data[vertex_count].data,
								buf+index, data32);
						}
						else
						{
							pmeta->vertex_data[vertex_count].data = 0;
						}
					}
					
					index += data32;

					if(i != (args_num-1))
					{
						index = ADJUST(index);
					}
					break;
				case ARG_TYPE:
					byte = buf[index++];

					if((cmd_type == CMD_GEN) && (i == 1 /* type */))
					{
						set_vertex_type(byte, vertex_count, matrix);
					}
					else if((cmd_type == CMD_GEN) && (i == 2 /* 3-d argument */))
					{
						pmeta->vertex_data[vertex_count].data_type = ARG_TYPE;
						pmeta->vertex_data[vertex_count].data_size = sizeof(BYTE);

						pmeta->vertex_data[vertex_count].data = 
							(BYTE*)malloc(sizeof(BYTE));

						if(pmeta->vertex_data[vertex_count].data == 0)
						{
							free(vertex_array);
							return MEMORY_ERROR;
						}

						memcpy(&(pmeta->vertex_data[vertex_count].data), &byte,
							sizeof(BYTE));
					}

					if(i != (args_num-1))
					{
						index = ADJUST(index);
					}
					break;
				case ARG_INT16:
					data32 = buf[index++];
					data32 += (buf[index++] << 8);

					if((cmd_type == CMD_GEN) && (i == 2 /* 3-d argument */))
					{
						pmeta->vertex_data[vertex_count].data_type = ARG_INT16;
						pmeta->vertex_data[vertex_count].data_size = sizeof(WORD);

						pmeta->vertex_data[vertex_count].data = 
							(BYTE*)malloc(sizeof(WORD));

						if(pmeta->vertex_data[vertex_count].data == 0)
						{
							free(vertex_array);
							return MEMORY_ERROR;
						}

						memcpy(&(pmeta->vertex_data[vertex_count].data), &byte,
							sizeof(WORD));
					}
					
					if(i != (args_num-1))
					{
						index += 2;
					}
					break;
				case ARG_STRING:
					data32 = buf[index++];
					data32 += (buf[index++] << 8);
					data32 += (buf[index++] << 16);
					data32 += (buf[index++] << 24);

					if((cmd_type == CMD_GEN) && (i == 0 /* idtf */))
					{
						pmeta->vertex_data[vertex_count].idtf_size = data32;

						if(data32)
						{
							pmeta->vertex_data[vertex_count].idtf = 
								(BYTE*)malloc(data32*sizeof(BYTE));

							if(pmeta->vertex_data[vertex_count].idtf == 0)
							{
								free(vertex_array);

								return MEMORY_ERROR;
							}

							memcpy(pmeta->vertex_data[vertex_count].idtf,
								buf+index, data32);
						}
						else
						{
							pmeta->vertex_data[vertex_count].idtf = 0;
						}
					}

					else if((cmd_type == CMD_GEN) && (i == 2 /* data */))
					{
						pmeta->vertex_data[vertex_count].data_size = data32;

						if(data32)
						{
							pmeta->vertex_data[vertex_count].data_type = ARG_STRING;

							pmeta->vertex_data[vertex_count].data = 
								(BYTE*)malloc(data32*sizeof(BYTE));

							if(pmeta->vertex_data[vertex_count].data == 0)
							{
								free(vertex_array);

								return MEMORY_ERROR;
							}

							memcpy(pmeta->vertex_data[vertex_count].data,
								buf+index, data32);
						}
						else
						{
							pmeta->vertex_data[vertex_count].data = 0;
						}
					}

					else if((cmd_type == CMD_DECLARE_PREFIX) && (i == 0 /* 1-st arg */))
					{
						pmeta->prefix_data[prefix_count].idtf_size = data32;

						if(data32)
						{
							pmeta->prefix_data[prefix_count].idtf = 
								(BYTE*)malloc(data32*sizeof(BYTE));

							if(pmeta->prefix_data[prefix_count].idtf == 0)
							{
								free(vertex_array);

								return MEMORY_ERROR;
							}

							memcpy(pmeta->prefix_data[prefix_count].idtf,
								buf+index, data32);
						}
						else
						{
							pmeta->prefix_data[prefix_count].idtf = 0;
						}
					}

					else if((cmd_type == CMD_FIND_BY_IDTF) && (i == 0 /* 1-st arg */))
					{
						pmeta->vertex_data[vertex_count].idtf_size = data32;

						if(data32)
						{
							pmeta->vertex_data[vertex_count].idtf = 
								(BYTE*)malloc(data32*sizeof(BYTE));

							if(pmeta->vertex_data[vertex_count].idtf == 0)
							{
								free(vertex_array);

								return MEMORY_ERROR;
							}

							memcpy(pmeta->vertex_data[vertex_count].idtf,
								buf+index, data32);
						}
						else
						{
							pmeta->vertex_data[vertex_count].idtf = 0;
						}

						pmeta->vertex_data[vertex_count].data_size = 0;
						pmeta->vertex_data[vertex_count].data = 0;
					}

					index += data32;
					
					if(i != (args_num-1))
					{
						index = ADJUST(index);
					}
					break;
				default:
					break;
			}

			if(index >= fsize)
				break;
		}

		if((cmd_type == CMD_GEN) || (cmd_type == CMD_FIND_BY_IDTF))
		{
			vertex_count++;
		}
		else if(cmd_type == CMD_DECLARE_PREFIX)
		{
			prefix_count++;
		}

		//skip checksum
		index += 1;

		index = ADJUST(index);

		if(index >= fsize)
			break;

		cmd_count++;
	}

	free(vertex_array);

	return OK;
}

//
void set_vertex_type(BYTE type, int vertex_num, BYTE **matrix)
{
	if((type & 1) == 1)
	{
		SET_TYPE(matrix, TYPE_FIELD1_1, vertex_num, 1);
	}
	if((type & 2) == 2)
	{
		SET_TYPE(matrix, TYPE_FIELD1_2, vertex_num, 1);
	}
	if((type & 3) == 3)
	{
		SET_TYPE(matrix, TYPE_FIELD1_3, vertex_num, 1);
	}
	if((type & 4) == 4)
	{
		SET_TYPE(matrix, TYPE_FIELD2_1, vertex_num, 1);
	}
	if((type & 8) == 8)
	{
		SET_TYPE(matrix, TYPE_FIELD2_2, vertex_num, 1);
	}
	if((type & 12) == 12)
	{
		SET_TYPE(matrix, TYPE_FIELD2_3, vertex_num, 1);
	}
	if((type & 16) == 16)
	{
		SET_TYPE(matrix, TYPE_FIELD3_1, vertex_num, 1);
	}
	if((type & 32) == 32)
	{
		SET_TYPE(matrix, TYPE_FIELD3_2, vertex_num, 1);
	}
	if((type & 48) == 48)
	{
		SET_TYPE(matrix, TYPE_FIELD3_3, vertex_num, 1);
	}
	if((type & 64) == 64)
	{
		SET_TYPE(matrix, TYPE_FIELD4_2, vertex_num, 1);
		SET_TYPE(matrix, TYPE_FIELD4_3, vertex_num, 1);
	}
	if((type & 128) == 128)
	{
		SET_TYPE(matrix, TYPE_FIELD4_4, vertex_num, 1);
//		SET_TYPE(matrix, TYPE_FIELD4_3, vertex_num, 0);
	}
	if((type & 192) == 0)
	{
		SET_TYPE(matrix, TYPE_FIELD4_1, vertex_num, 1);
		SET_TYPE(matrix, TYPE_FIELD4_3, vertex_num, 1);
	}
}

//
void get_vertex_type(BYTE *type, int vertex_num, BYTE **matrix)
{
	BYTE val = 0;

	*type = 0;

	GET_TYPE(matrix, TYPE_FIELD1_1, vertex_num, val);
	if(val)	{ *type |= 1; }

	GET_TYPE(matrix, TYPE_FIELD1_2, vertex_num, val);
	if(val)	{ *type |= 2; }

	GET_TYPE(matrix, TYPE_FIELD1_3, vertex_num, val);
	if(val)	{ *type |= 3; }

	GET_TYPE(matrix, TYPE_FIELD2_1, vertex_num, val);
	if(val)	{ *type |= 4; }

	GET_TYPE(matrix, TYPE_FIELD2_2, vertex_num, val);
	if(val)	{ *type |= 8; }

	GET_TYPE(matrix, TYPE_FIELD2_3, vertex_num, val);
	if(val)	{ *type |= 12; }

	GET_TYPE(matrix, TYPE_FIELD3_1, vertex_num, val);
	if(val)	{ *type |= 16; }

	GET_TYPE(matrix, TYPE_FIELD3_2, vertex_num, val);
	if(val)	{ *type |= 32; }

	GET_TYPE(matrix, TYPE_FIELD3_3, vertex_num, val);
	if(val)	{ *type |= 48; }

	GET_TYPE(matrix, TYPE_FIELD4_1, vertex_num, val);
	if(val)	{ *type |= 0; }

	GET_TYPE(matrix, TYPE_FIELD4_2, vertex_num, val);
	if(val)	{ *type |= 64; }

	GET_TYPE(matrix, TYPE_FIELD4_4, vertex_num, val);
	if(val)	{ *type |= 128; }
}

//
int write_f3(const char *fname, F3_HEADER *pheader, BYTE **matrix,
			 METADATA *pmeta, UINT *  index)
{
	BYTE reserved[] = {0x00, 0x00, 0x00};
	int rv;
	WORD buf;

	FILE *out = fopen(fname, "wb");

	if(!out)
	{
		return FILE_ERROR;
	}

	fwrite(pheader, 1, 8*sizeof(BYTE), out);
	fwrite(reserved, 1, sizeof(BYTE), out);
	fwrite(&(pheader->checksumming), 1, sizeof(BYTE), out);
	fwrite(reserved, 1, 2*sizeof(BYTE), out);
	fwrite(&(pheader->descr_size), 1, sizeof(BYTE), out);
	fwrite(pheader->description, 1, pheader->descr_size*sizeof(BYTE), out);

	adjust_file(out);

	rv = write_f3_file_crc(out, matrix, pmeta, index);

	if(rv == OK)
	{
		adjust_file(out);

		buf = CMD_END_OF_STREAM;
		fwrite(&buf, 1, sizeof(WORD), out);

		buf = 0;
		fwrite(&buf, 1, sizeof(WORD), out);

		adjust_file(out);

		// crc (crc = END_OF_STREAM)
		buf = CMD_END_OF_STREAM;
		fwrite(&((BYTE)buf), 1, sizeof(BYTE), out);
	}
	
	fclose(out);

	return rv;
}

//
void adjust_file(FILE *f)
{
	BYTE empty = 0x00;
	long x = ftell(f);
	long i;
	long end = ADJUST(x);

	for(i = 0; i < (end - x); i++)
	{
		fwrite(&empty, 1, sizeof(BYTE), f);
	}
}

//
int write_f3_file_crc(FILE *out, BYTE **matrix, METADATA *pmeta, UINT * index)
{
	int i, j, i_, j_;
	int arg_type, int32, cmd_count = 0;
	int current_prefix_id = -1;
	int vertex1, vertex2;
	WORD cmd, args_cnt;
	BYTE type, crc, crc_const = 0;
	BYTE val1, val2;

	int *vertex_array = (int*)malloc(pmeta->vertex_num*sizeof(int));

	if(!vertex_array)
	{
		return MEMORY_ERROR;
	}

	if(pmeta->prefix_num)
	{
		cmd = CMD_DECLARE_PREFIX;
		args_cnt = 1;
		arg_type = ARG_STRING;

		crc_const ^= (BYTE)cmd;
		crc_const ^= (BYTE)(cmd >> 8);
		crc_const ^= (BYTE)args_cnt;
		crc_const ^= (BYTE)(args_cnt >> 8);
		crc_const ^= (BYTE)(arg_type);
		crc_const ^= (BYTE)(arg_type >> 8);
		crc_const ^= (BYTE)(arg_type >> 16);
		crc_const ^= (BYTE)(arg_type >> 24);
	}

	for(i = 0; i < pmeta->prefix_num; i++)
	{
		adjust_file(out);
		crc = crc_const;

		fwrite(&cmd, 1, sizeof(WORD), out);
		fwrite(&args_cnt, 1, sizeof(WORD), out);

		adjust_file(out);

		fwrite(&arg_type, 1, sizeof(int), out);
		fwrite(&(pmeta->prefix_data[i].idtf_size), 1, sizeof(int), out);

		crc ^= (BYTE)(pmeta->prefix_data[i].idtf_size);
		crc ^= (BYTE)((pmeta->prefix_data[i].idtf_size) >> 8);
		crc ^= (BYTE)((pmeta->prefix_data[i].idtf_size) >> 16);
		crc ^= (BYTE)((pmeta->prefix_data[i].idtf_size) >> 24);

		for(j = 0; j < pmeta->prefix_data[i].idtf_size; j++)
		{
			fwrite(&(pmeta->prefix_data[i].idtf[j]), 1, sizeof(BYTE), out);

			crc ^= (BYTE)(pmeta->prefix_data[i].idtf[j]);
		}

		fwrite(&crc, 1, sizeof(BYTE), out);

		current_prefix_id = i;

		cmd_count++;
	}

	for(i_ = 0; i_ < pmeta->vertex_num; i_++)
	{
		i = (index == NULL)? i_: index[i_];
		if(current_prefix_id != pmeta->vertex_data[i].prefix_id)
		{
			adjust_file(out);

			current_prefix_id = pmeta->vertex_data[i].prefix_id;

			cmd = CMD_SWITCH_PREFIX;
			args_cnt = 1;
			arg_type = ARG_INT32;
			int32 = current_prefix_id;

			fwrite(&cmd, 1, sizeof(WORD), out);
			fwrite(&args_cnt, 1, sizeof(WORD), out);

			adjust_file(out);

			fwrite(&arg_type, 1, sizeof(int), out);
			fwrite(&int32, 1, sizeof(int), out);

			crc = 0;

			crc ^= (BYTE)cmd;
			crc ^= (BYTE)(cmd >> 8);
			crc ^= (BYTE)args_cnt;
			crc ^= (BYTE)(args_cnt >> 8);
			crc ^= (BYTE)(arg_type);
			crc ^= (BYTE)(arg_type >> 8);
			crc ^= (BYTE)(arg_type >> 16);
			crc ^= (BYTE)(arg_type >> 24);
			crc ^= (BYTE)(int32);
			crc ^= (BYTE)(int32 >> 8);
			crc ^= (BYTE)(int32 >> 16);
			crc ^= (BYTE)(int32 >> 24);

			fwrite(&crc, 1, sizeof(BYTE), out);

			cmd_count++;
		}

		adjust_file(out);

		crc = 0;

		cmd = CMD_GEN;
		args_cnt = pmeta->vertex_data[i].data_size ? 3 : 2;

		crc ^= (BYTE)cmd;
		crc ^= (BYTE)(cmd >> 8);
		crc ^= (BYTE)args_cnt;
		crc ^= (BYTE)(args_cnt >> 8);

		fwrite(&cmd, 1, sizeof(WORD), out);
		fwrite(&args_cnt, 1, sizeof(WORD), out);

		adjust_file(out);

		arg_type = ARG_STRING;

		fwrite(&arg_type, 1, sizeof(int), out);
		fwrite(&(pmeta->vertex_data[i].idtf_size), 1, sizeof(int), out);

		crc ^= (BYTE)(arg_type);
		crc ^= (BYTE)(arg_type >> 8);
		crc ^= (BYTE)(arg_type >> 16);
		crc ^= (BYTE)(arg_type >> 24);

		crc ^= (BYTE)(pmeta->vertex_data[i].idtf_size);
		crc ^= (BYTE)((pmeta->vertex_data[i].idtf_size) >> 8);
		crc ^= (BYTE)((pmeta->vertex_data[i].idtf_size) >> 16);
		crc ^= (BYTE)((pmeta->vertex_data[i].idtf_size) >> 24);

		for(j = 0; j < pmeta->vertex_data[i].idtf_size; j++)
		{
			fwrite(&(pmeta->vertex_data[i].idtf[j]), 1, sizeof(BYTE), out);

			crc ^= (BYTE)(pmeta->vertex_data[i].idtf[j]);
		}

		adjust_file(out);

		arg_type = ARG_TYPE;

		fwrite(&arg_type, 1, sizeof(int), out);

		crc ^= (BYTE)(arg_type);
		crc ^= (BYTE)(arg_type >> 8);
		crc ^= (BYTE)(arg_type >> 16);
		crc ^= (BYTE)(arg_type >> 24);

		get_vertex_type(&type, i, matrix);
		fwrite(&type, 1, sizeof(BYTE), out);

		crc ^= type;

		if(pmeta->vertex_data[i].data_size)
		{
			adjust_file(out);

			arg_type = pmeta->vertex_data[i].data_type;
			fwrite(&arg_type, 1, sizeof(int), out);

			crc ^= (BYTE)(arg_type);
			crc ^= (BYTE)(arg_type >> 8);
			crc ^= (BYTE)(arg_type >> 16);
			crc ^= (BYTE)(arg_type >> 24);

			if((pmeta->vertex_data[i].data_type == ARG_DATA) ||
				(pmeta->vertex_data[i].data_type == ARG_STRING))
			{
				fwrite(&(pmeta->vertex_data[i].data_size), 1, sizeof(int), out);

				crc ^= (BYTE)(pmeta->vertex_data[i].data_size);
				crc ^= (BYTE)((pmeta->vertex_data[i].data_size) >> 8);
				crc ^= (BYTE)((pmeta->vertex_data[i].data_size) >> 16);
				crc ^= (BYTE)((pmeta->vertex_data[i].data_size) >> 24);
			}

			for(j = 0; j < pmeta->vertex_data[i].data_size; j++)
			{
				fwrite(&(pmeta->vertex_data[i].data[j]), 1, sizeof(BYTE), out);

				crc ^= (BYTE)(pmeta->vertex_data[i].data[j]);
			}
		}

		fwrite(&crc, 1, sizeof(BYTE), out);

		vertex_array[i] = cmd_count;

		cmd_count++;
	}

	for(i_ = 0; i_ < pmeta->vertex_num; i_++)
	{
		i = (index == NULL)? i_: index[i_];
		for(j_ = i_; j_ < pmeta->vertex_num; j_++)
		{
			j = (index == NULL)? j_: index[j_];
			GET_ELEMENT(matrix, i, j, val1);
			GET_ELEMENT(matrix, j, i, val2);

			if((!val1) && (!val2))
			{
				continue;
			}

			cmd = (val1 && val2) ? CMD_SET_BEG : CMD_SET_END;

			if(val1)
			{
				vertex1 = vertex_array[i];
				vertex2 = vertex_array[j];
			}
			else
			{
				vertex1 = vertex_array[j];
				vertex2 = vertex_array[i];
			}

			adjust_file(out);

			crc = 0;

			args_cnt = 2;
			arg_type = ARG_INT32;

			fwrite(&cmd, 1, sizeof(WORD), out);
			fwrite(&args_cnt, 1, sizeof(WORD), out);

			crc ^= (BYTE)cmd;
			crc ^= (BYTE)(cmd >> 8);
			crc ^= (BYTE)args_cnt;
			crc ^= (BYTE)(args_cnt >> 8);

			adjust_file(out);

			int32 = vertex1;
			fwrite(&arg_type, 1, sizeof(int), out);
			fwrite(&int32, 1, sizeof(int), out);

			crc ^= (BYTE)(arg_type);
			crc ^= (BYTE)(arg_type >> 8);
			crc ^= (BYTE)(arg_type >> 16);
			crc ^= (BYTE)(arg_type >> 24);
			crc ^= (BYTE)(int32);
			crc ^= (BYTE)(int32 >> 8);
			crc ^= (BYTE)(int32 >> 16);
			crc ^= (BYTE)(int32 >> 24);

			adjust_file(out);

			int32 = vertex2;
			fwrite(&arg_type, 1, sizeof(int), out);
			fwrite(&int32, 1, sizeof(int), out);

			crc ^= (BYTE)(arg_type);
			crc ^= (BYTE)(arg_type >> 8);
			crc ^= (BYTE)(arg_type >> 16);
			crc ^= (BYTE)(arg_type >> 24);
			crc ^= (BYTE)(int32);
			crc ^= (BYTE)(int32 >> 8);
			crc ^= (BYTE)(int32 >> 16);
			crc ^= (BYTE)(int32 >> 24);

			fwrite(&crc, 1, sizeof(BYTE), out);

			cmd_count++;
		}
	}

	free(vertex_array);

	return OK;
}
