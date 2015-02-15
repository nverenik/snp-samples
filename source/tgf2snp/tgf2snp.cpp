///////////////////////////////////////////////////////////////////////
// File:	tgf2snp.cpp
// Date:	9.02.2015
// Author:	Valerian Ivashenko	
///////////////////////////////////////////////////////////////////////
// Description:	unified semantic knowledge representation model utility
//		TGF to sematic network processor translator sample
// tgf2snp.exe
//	<file>	-	input tgf file
//

#include <fstream>
#include <iostream>
using namespace std;
#include <windows.h>
#include <tchar.h>

const unsigned fixsize	=	4096;
TCHAR help[fixsize]	= _T("<file>	-	input file");
#include "sctranslator_snp.h"

int main (unsigned int argc, char *  argv[]) {
	TCHAR *  file;
	if (argc < 2) {cout << help; return -1;}
	else file = argv[1];
	sc_access_levels access_levels;
	translate_snp(file,access_levels);
	return 0;
}