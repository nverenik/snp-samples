///////////////////////////////////////////////////////////////////////
// File:	sctext.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#ifndef __SCTEXT_H__
#define __SCTEXT_H__

#include <windows.h>

#include "dtype.h"

extern "C" {
#include "types.h"
#include "convertor.h"
#include "metadata.h"
#include "matrix.h"
}
typedef unsigned telement;

struct ttext {
 	BYTE ** adj;
	METADATA metadata;
	telement * index;
	telement l;
	telement u;
	telement b;
	telement size;
	telement e;
	bool f,t;
	ttext() {
		metadata.vertex_num = 0; 
		metadata.prefix_num = 0; 
		metadata.prefix_data = NULL; 
		metadata.vertex_data = NULL;
		adj = NULL; 
		index = NULL;
		b = 0;
		l = 0;
		u = 0;
		size = 0;
		e = 0;
		f = true;
		t = false;
	};
	ttext(BYTE ** a, METADATA& m) {
		metadata = m; 
		adj = a;
		b = l = 0;
		u = size = metadata.vertex_num;
		index = new telement[size];
		for (unsigned i = 0; i < size; i++) index[i] = i;
		e = 0;
		f = true;
		t = false;
	};
	~ttext() {
		if (index != NULL) delete[] index;
	};
};

struct ttextselector {
 	ttext *  ptext;
	ttextselector() {	ptext = NULL;	};
	ttextselector(ttextselector& t) {	ptext = (t.ptext == NULL)? ptext: t.ptext; 	};
	ttextselector(ttext& p) {	ptext = &p;	};
	ttextselector& operator =(ttextselector& t) {	ptext = (t.ptext == NULL)? ptext: t.ptext; return *this;	};
	bool operator <(ttextselector t) {	
		ptext = (ptext == NULL)? t.ptext: ptext;

		bool r = false;
		if (ptext->f) {
			if (r = (ptext->b < ptext->u)) {
				ptext->b = ptext->l;
				ptext->u = ptext->size;
				ptext->f = false;
			}
			else {
				ptext->b = 0;
				ptext->t = true;
			}
		}
		else if (r = ((ptext->t?ptext->size:ptext->u) > ptext->l));
		else {
			ptext->f = true;
			ptext->l = ptext->u;
		}// r = false
		return	r;
	};
	ttextselector& operator >>(telement& e) {	e = ptext->l++; return *this;	};
	ttextselector& operator <<(telement) {	ptext->u--;	return *this;	};
};

typedef element_type ttype;

struct tidentifier {
	unsigned size, prefix_size;
	BYTE *  identifier, *  prefix;
};

struct tcontent {
	unsigned type, size;
	BYTE *  content;
};

telement fbeg(ttext& x,telement e);
telement fend(ttext& x,telement e);
ttype ftype_constancy(ttext& x,telement e);
ttype ftype(ttext& x,telement e);
tidentifier fidentifier(ttext& t,telement e);
tcontent fcontent(ttext& t,telement e);
bool fedges(ttext& t,telement e);
bool fidentified(ttext& t,telement e);
bool fcontained(ttext& t,telement e);
bool ffalse(ttext& t,telement e);
bool ftrue(ttext& t,telement e);
#endif//__SCTEXT_H__
