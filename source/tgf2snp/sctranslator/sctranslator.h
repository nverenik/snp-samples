///////////////////////////////////////////////////////////////////////
// File:	sctranslator.h
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility

template <	typename TELEMENT,
			typename TTYPE,
			typename TCODE,
			typename TSTRUCTURE,
			TSTRUCTURE	(*SET_END)		(TCODE,TSTRUCTURE,TSTRUCTURE),
			TSTRUCTURE	(*GEN_BEG)		(TCODE,TELEMENT,TTYPE,TSTRUCTURE)>
TSTRUCTURE gen_beg_end (TCODE C,TELEMENT M,TTYPE T,TSTRUCTURE B,TSTRUCTURE E) {	return SET_END(C,GEN_BEG(C,M,T,B),E);	}

template <	typename TELEMENT,
			typename TTYPE,
			typename TCODE,
			typename TSTRUCTURE,
			TSTRUCTURE	(*SET_END)		(TCODE,TSTRUCTURE,TSTRUCTURE),
			TSTRUCTURE	(*GEN_BEG)		(TCODE,TELEMENT,TTYPE,TSTRUCTURE)>
TSTRUCTURE gen_loop (TCODE C,TELEMENT M,TTYPE T,TSTRUCTURE B,TELEMENT E) {	TSTRUCTURE& S = GEN_BEG(C,M,T,B);	return SET_END(C,S,S);	}

template <	typename TELEMENT,
			typename TTYPE,
			typename TCODE,
			typename TSTRUCTURE,
			TSTRUCTURE	(*SET_END)		(TCODE,TSTRUCTURE,TSTRUCTURE),
			TSTRUCTURE	(*GEN_BEG)		(TCODE,TELEMENT,TTYPE,TSTRUCTURE),
			typename TRSTRUCTURER>
TSTRUCTURE gen_loop (TCODE C,TELEMENT M,TTYPE T,TSTRUCTURE B,TELEMENT E) {	TSTRUCTURER S = GEN_BEG(C,M,T,B);	return SET_END(C,S,S);	}

template <	typename TTEXT, 
			typename TTEXTSELECTOR,
			typename TELEMENT,
			typename TTYPE,
			typename TIDENTIFIER,
			typename TCONTENT,
			typename TCODE,
			typename TSTRUCTURE,
			typename TMAP,
			typename TRVALUE,
			bool		(*EDGES)		(TTEXT,TELEMENT),
			bool		(*IDENTIFIED)	(TTEXT,TELEMENT),
			bool		(*CONTAINED)	(TTEXT,TELEMENT),
			TTYPE		(*TYPE)			(TTEXT,TELEMENT),
			TIDENTIFIER	(*IDENTIFIER)	(TTEXT,TELEMENT),
			TCONTENT	(*CONTENT)		(TTEXT,TELEMENT),
			TELEMENT	(*BEG)			(TTEXT,TELEMENT),
			TELEMENT	(*END)			(TTEXT,TELEMENT),
			TSTRUCTURE	(*GEN)			(TCODE,TELEMENT,TTYPE),
			TSTRUCTURE	(*SET_END)		(TCODE,TSTRUCTURE,TSTRUCTURE),
			TSTRUCTURE	(*IDENTIFY)		(TCODE,TSTRUCTURE,TIDENTIFIER),
			TSTRUCTURE	(*CONTAIN)		(TCODE,TSTRUCTURE,TCONTENT),
			TSTRUCTURE	(*GEN_BEG)		(TCODE,TELEMENT,TTYPE,TSTRUCTURE),
			TSTRUCTURE	(*GEN_BEG_END)	(TCODE,TELEMENT,TTYPE,TSTRUCTURE,TSTRUCTURE),
			TSTRUCTURE	(*GEN_LOOP)		(TCODE,TELEMENT,TTYPE,TSTRUCTURE,TELEMENT),
			bool		(*MAPPED)		(TELEMENT,TMAP),
			TSTRUCTURE	(*GET)			(TELEMENT,TMAP),
			TMAP		(*MAP)			(TELEMENT,TSTRUCTURE,TMAP),
			TRVALUE		(*FINALIZER)	(TTEXT,TCODE,TMAP)>
TRVALUE sctranslate (TTEXT T, TCODE C, TMAP M) {
	TTEXTSELECTOR E;
	TTEXTSELECTOR X = T;
	TTEXTSELECTOR Y;
	TTEXTSELECTOR Z = X;
    while (Y < Z) {
		Z = X;
		Y = E;
		while (E < X) {
			TELEMENT x;
			X >> x;
			TTYPE t = TYPE(T,x);
			if (EDGES(T,x)) {
				TELEMENT b = BEG(T,x);
				if (MAPPED(b,M)) {
					TELEMENT e = END(T,x);
					if (MAPPED(e,M)) MAP(x,GEN_BEG_END(C,x,t,GET(b,M),GET(e,M)),M);
					else if (x == e) MAP(x,GEN_LOOP(C,x,t,GET(b,M),x),M);
					else Y << x;
				}
				else Y << x;
			}
			else MAP(x,GEN(C,x,t),M);
			if (MAPPED(x,M)) {
				if (IDENTIFIED(T,x)) IDENTIFY(C,GET(x,M),IDENTIFIER(T,x));
				if (CONTAINED(T,x)) CONTAIN(C,GET(x,M),CONTENT(T,x));
			}
		}
		X = Y;
	}
	Y = E;
	while (Y < Z) {
		Z = X;
		Y = E;
		while (E < Y) {
			TELEMENT x;
			X >> x;
			TTYPE t = TYPE(T,x);
			if (EDGES(T,x)) {
				TELEMENT b = BEG(T,x);
				if (MAPPED(b,M)) {
					TELEMENT e = END(T,x);
					if (MAPPED(e,M)) {
						if (MAPPED(x,M)) SET_END(C,GET(x,M),GET(e,M));
						else MAP(x,GEN_BEG_END(C,x,t,GET(b,M),GET(e,M)),M);
						if (IDENTIFIED(T,x)) IDENTIFY(C,GET(x,M),IDENTIFIER(T,x));
						if (CONTAINED(T,x)) CONTAIN(C,GET(x,M),CONTENT(T,x));
					}
					else {
						MAP(x,GEN_BEG(C,x,t,GET(b,M)),M);
						Y << x;
					}
				}
			}
		}
		X = Y;
	}
	return FINALIZER(T,C,M);
}