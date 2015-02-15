///////////////////////////////////////////////////////////////////////
// File:	matrix.c
// Date:	13.12.2005
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "matrix.h"
#include "common.h"

//
int allocate_matrix(BYTE ***matrix, METADATA *pmeta)
{
	int i, j;
	int num = pmeta->vertex_num + TYPES_CNT;

	*matrix = (BYTE**)malloc(num*sizeof(BYTE*));

	if(!(*matrix))
	{
		return MEMORY_ERROR;
	}

	for(i = 0; i < num; i++)
	{
		(*matrix)[i] = (BYTE*)malloc(num/8 + ((num % 8) ? 1 : 0));

		if(!(*matrix)[i])
		{
			for(j = 0; j < i; j++)
			{
				free((*matrix)[j]);
			}
			free(*matrix);

			return MEMORY_ERROR;
		}

		for(j = 0; j < (num/8 + ((num % 8) ? 1 : 0)); j++)
		{
			(*matrix)[i][j] = 0;
		}
	}

	return OK;
}

//
void delete_matrix(BYTE ***matrix, METADATA *pmeta)
{
	int i;
	int num = pmeta->vertex_num + TYPES_CNT;

	if(!(*matrix))
	{
		return;
	}

	for(i = 0; i < num; i++)
	{
		free((*matrix)[i]);
	}

	free(*matrix);

	*matrix = 0;
}
