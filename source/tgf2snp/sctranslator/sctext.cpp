///////////////////////////////////////////////////////////////////////
// File:	sctext.cpp
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

#include "sctext.h"

telement fbeg(ttext& x,telement e) {
	unsigned i,t;
	x.e = e;
	for (i = 0; i < x.size; i++) {
		GET_ELEMENT(x.adj,x.index[e],x.index[i],t);
		if (t != 0) {
			GET_ELEMENT(x.adj,x.index[i],x.index[e],t);
			if (t != 0) break;
			else x.e = i;
		}
	}
	GET_ELEMENT(x.adj,x.index[e],x.index[x.e],t);
	if (t == 0) x.e = i;
	if (i > e) {
		i = ((e < x.e) && (x.e < i))? x.e: i;
		x.e = x.index[e];
		x.index[e] = x.index[i];
		x.index[i] = x.e;
		x.l--;
		i = x.size;
	}
	return i;
};
telement fend(ttext& x,telement e) {
	unsigned i,t;
	i = x.e;
	GET_ELEMENT(x.adj,x.index[e],x.index[i],t);
	if (t != 0) {
		GET_ELEMENT(x.adj,x.index[i],x.index[e],t);
		if (t != 0) {
			for (++i; i < x.size; i++) {
				GET_ELEMENT(x.adj,x.index[e],x.index[i],t);
				if (t != 0) break;
			}
		}
	}
	i = (i == x.size)? x.e: i;
	if ((i > e) && !x.t) {
		x.e = x.index[e];
		x.index[e] = x.index[i];
		x.index[i] = x.e;
		x.l--;
		i = x.size;
	}
	return i;
};
ttype ftype(ttext& x,telement e,ttype type) {
	unsigned int t, i = x.index[e];
	GET_TYPE(x.adj,TYPE_FIELD2_1,i,t); //pos
	type = element_type((t != 0)? type | POSITIVE: type);
	GET_TYPE(x.adj,TYPE_FIELD2_2,i,t); //neg
	type = element_type((t != 0)? type | NEGATIVE: type);
	GET_TYPE(x.adj,TYPE_FIELD2_3,i,t); //fuz
	type = element_type((t != 0)? type | FUZZY: type);
	GET_TYPE(x.adj,TYPE_FIELD3_1,i,t); //arc
	type = element_type((t != 0)? type | ARC: type);
	GET_TYPE(x.adj,TYPE_FIELD3_2,i,t); //node
	type = element_type((t != 0)? type | NODE: type);
	GET_TYPE(x.adj,TYPE_FIELD3_3,i,t); //uncertained
	type = element_type((t != 0)? type | UNCERTAINED: type);
	GET_TYPE(x.adj,TYPE_FIELD4_1,i,t); //actual
	type = element_type((t != 0)? type | ACTUAL: type);
	GET_TYPE(x.adj,TYPE_FIELD4_2,i,t); //phantom
	type = element_type((t != 0)? type | PHANTOM: type);
	GET_TYPE(x.adj,TYPE_FIELD4_3,i,t); //temporary
	type = element_type((t != 0)? type | TEMPORARY: type);
	GET_TYPE(x.adj,TYPE_FIELD4_4,i,t); //permanent
	type = element_type((t != 0)? type | PERMANENT: type);
	switch(type&TOPOLOGY) {
				case(TOPOLOGY):
					type = element_type((type & (NODE|ARC)) ^ type);
				case(UNCERTAINED):
				case(NODE):
					type = element_type((type & (TEMPORARY|ACTUAL)) ^ type);
	}
	return type;
};
ttype ftype(ttext& x,telement e) {	return ftype(x,e,EMPTY);	};
ttype ftype_constancy(ttext& x,telement e) {
	unsigned int t, i = x.index[e];
	ttype type = EMPTY;
	ttype META = element_type(1<<F_LINK);
	ttype VAR = element_type(1<<F_AREA);
#ifdef  CONST
#undef  CONST
#endif//CONST
	ttype CONST = element_type(1<<F_SEGMENT);
	GET_TYPE(x.adj,TYPE_FIELD1_1,i,t); //const
	type = element_type((t != 0)? type | CONST: type);
	GET_TYPE(x.adj,TYPE_FIELD1_2,i,t); //var
	type = element_type((t != 0)? type | VAR: type);
	GET_TYPE(x.adj,TYPE_FIELD1_3,i,t); //meta
	type = element_type((t != 0)? type | META: type);
	return ftype(x,e,type);
};
tidentifier fidentifier(ttext& t,telement e) {
	tidentifier i;
	unsigned d	= t.metadata.vertex_data[t.index[e]].prefix_id;
	i.size		= t.metadata.vertex_data[t.index[e]].idtf_size;
	i.identifier= t.metadata.vertex_data[t.index[e]].idtf;
	if (d < unsigned(t.metadata.prefix_num)) {
		i.prefix_size=t.metadata.prefix_data[d].idtf_size;
		i.prefix	= t.metadata.prefix_data[d].idtf;
	}
	return i;
};
tcontent fcontent(ttext& t,telement e) {
	tcontent c;
	c.size		= t.metadata.vertex_data[t.index[e]].data_size;
	c.type		= t.metadata.vertex_data[t.index[e]].data_type;
	c.content	= t.metadata.vertex_data[t.index[e]].data;
	return c;
};
bool fedges(ttext& t,telement e) { return (ftype(t,e)&ARC) == ARC; };
bool fidentified(ttext& t,telement e) { return (0 < unsigned(t.metadata.vertex_data[t.index[e]].idtf_size)); };
bool fcontained(ttext& t,telement e) { return (0 < unsigned(t.metadata.vertex_data[t.index[e]].data_size)); };
bool ffalse(ttext& t,telement e) { return false; };
bool ftrue(ttext& t,telement e) { return true; };
