#ifndef __PF_SOLVER_H__
#define __PF_SOLVER_H__

#include <snp/Macros.h>
class CGraph;

class PFSolver
{
public:
    static bool FindPath(CGraph *pGraph, uint32 uiSrcID, uint32 uiDstID);
};

#endif //__PF_SOLVER_H__