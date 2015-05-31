 
/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/

#include <TAXI/Tbase.h>
#include <TAXI/Tsvector.h>
#include <TAXI/graphs.h>
#if defined(_WINDOWS) || defined(_WIN32)
#define srand48 srand
#define lrand48 rand
#endif


long & randomSetSeed()
  {static long _seed = 1;
  return _seed;
  }
void randomInitSeed()
// called once, when program  starts
  {time_t time_seed;
  time(&time_seed);
  randomSetSeed() = (long)time_seed; 
  }
void randomStart()
  {srand48(randomSetSeed());
  }
void randomEnd()
  {randomSetSeed() = lrand48();
  }
long randomGet(long range) 
//returns an integer >= 1 && <= range
  {return (lrand48()%(long)range) +1;
  }
void randomShuffle(svector<int> &tab)
// randomly permute the elements of tab
 {int n = tab.n();
 if(n < 2)return;
 randomStart();
 for(int i = 0;i < n;i++)
     {int r = i + (int) (lrand48()%(long)(n-i));
     if(i == r)continue;
     tab.SwapIndex(i,r);
     }
 randomEnd();
 }
void shuffleCir(TopologicalGraph &G)
  {G.planarMap() = 0;
  for(tvertex v = 1; v < G.nv();v++)
      {int degree = G.Degree(v);
      if(degree < 3)continue;
      svector<int> tab(degree);
      tbrin e0 = G.pbrin[v];
      tbrin e = e0;
      int d = 0;
      do
          {tab[d++] = e();
          }while((e = G.cir[e]) != e0);
      randomShuffle(tab);
      for(d = 0;d < degree-1;d++)
          G.cir[tab[d]] = (tbrin)tab[d+1];
      G.cir[tab[degree-1]] = (tbrin)tab[0];
      G.pbrin[v] = (tbrin)tab[0];
      }
  // Compute acir
  for(tbrin b = -G.ne(); b<= G.ne(); b++)
      G.acir[G.cir[b]] = b;
  }

