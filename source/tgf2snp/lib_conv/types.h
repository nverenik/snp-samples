///////////////////////////////////////////////////////////////////////
// File:	types.h
// Date:	8.03.2007
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _TYPES_H_
#define _TYPES_H_

// types
typedef unsigned int UINT;
#define BYTE			unsigned char
#define WORD			unsigned short
#define DWORD			unsigned int

// formats
#define F3_FORMAT		1
#define DG6_FORMAT		2
#define G6_FORMAT		3
#define S6_FORMAT		4
#define INVALID_FORMAT	-1

// error codes
#define OK				1
#define MEMORY_ERROR	-1
#define FILE_ERROR		-2
#define CRC_NOT_OK		-3
#define UNDEF_ERROR		-4
#define INVALID_HEADER	-5
#ifdef  FAILED
#undef  FAILED
#endif//FAILED
#define FAILED			UNDEF_ERROR

// number of f3 types
#define TYPES_CNT		13

// f3 types
#define TYPE_FIELD1_1	0
#define TYPE_FIELD1_2	1
#define TYPE_FIELD1_3	2

#define TYPE_FIELD2_1	3
#define TYPE_FIELD2_2	4
#define TYPE_FIELD2_3	5

#define TYPE_FIELD3_1	6
#define TYPE_FIELD3_2	7
#define TYPE_FIELD3_3	8

#define TYPE_FIELD4_1	9
#define TYPE_FIELD4_2	10
#define TYPE_FIELD4_3	11
#define TYPE_FIELD4_4	12

// prefix
#define EMPTY_PREFIX	-1

// format 3.2 commands
#define CMD_NOP					0
#define CMD_GEN					1
#define CMD_DECLARE_PREFIX		2
#define CMD_SWITCH_PREFIX		3
#define CMD_SET_BEG				4
#define CMD_SET_END				5
#define CMD_FIND_BY_IDTF		6
#define CMD_END_OF_STREAM		7

// format 3.2 arguments
#define ARG_INT32		0
#define ARG_INT64		1
#define ARG_FLOAT		2
#define ARG_DATA		3
#define ARG_TYPE		4
#define ARG_INT16		5
#define ARG_STRING		6
#define ARG_UNDEF		-1

// types
typedef struct _FORMAT3_HEADER_
{
	BYTE signature[3];
	BYTE minor_version;
	BYTE major_version;
	BYTE endianness;
	WORD compression;
	BYTE checksumming;
	BYTE descr_size;
	char description[256];
	WORD size;
}
F3_HEADER;

typedef struct _VERTEX_DATA_
{
	BYTE *idtf;
	BYTE *data;
	int prefix_id;
	int data_type;

	int idtf_size;
	int data_size;
}
DATA;

typedef struct _PREFIX_
{
	BYTE *idtf;
	int id;

	int idtf_size;
}
PREFIX;

typedef struct _METADATA_
{
	int vertex_num;
	int prefix_num;
	DATA *vertex_data;
	PREFIX *prefix_data;
}
METADATA;

#endif //_TYPES_H_
