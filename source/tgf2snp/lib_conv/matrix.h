///////////////////////////////////////////////////////////////////////
// File:	matrix.h
// Date:	3.05.2007
// Author:	Valerian Ivashenko	
// Coded by:	CORSAIR
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "types.h"

#define V(x)	(x + TYPES_CNT)
	
#define SET_ELEMENT(matrix, i, j, val)				\
			{										\
				if(val)								\
				{									\
					(matrix)[V(i)][V(j)/8] |=		\
						(1 << (V(j) % 8));			\
				}									\
				else								\
				{									\
					(matrix)[V(i)][V(j)/8] &=		\
						(0xFF & (0 << (V(j) % 8)));	\
				}									\
			}

#define GET_ELEMENT(matrix, i, j, val)				\
			{										\
				val = (((matrix)[V(i)][V(j)/8] &	\
					(1 << (V(j) % 8))) ? (1) : (0));\
			}

#define SET_TYPE(matrix, type, element, val)		\
													\
			SET_ELEMENT(matrix, type - TYPES_CNT,	\
				element, val)

#define GET_TYPE(matrix, type, element, val)		\
													\
			GET_ELEMENT(matrix, type - TYPES_CNT,	\
				element, val)


int allocate_matrix(BYTE ***, METADATA *);
void delete_matrix(BYTE ***, METADATA *);

#endif _MATRIX_H_
