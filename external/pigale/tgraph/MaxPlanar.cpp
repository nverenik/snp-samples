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


#include <TAXI/graph.h>
#include <TAXI/color.h>
#include <TAXI/DFSGraph.h>


tedge EraseMostMarkedEdge(TopologicalGraph& G0,svector<int>& count)
  {tedge e,e0,f0;
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,G0);
  DG.CotreeCritical();
  DG.Kuratowski();
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK); 
  int maxcount=0;
  e0 = e = 0;
  for(tedge j = DG.ne();j > 0;j--)
      {if(!keep[j])continue;
      f0 = DG.ie(j);   // e0 belongs to G0  
      if (++count[f0]>maxcount) 
          {maxcount = count[f0];
          e0 = f0;
          e = initedge[e0];// e belongs to G
          }
      }
  if (e0!=0) G0.DeleteEdge(e0);
  return e;
  }
void ReinsertSomeEdges(TopologicalGraph& G,TopologicalGraph& G0
                       ,svector<tedge>& tab,int& n)
  {tedge e,e0;
  int nn = n;
  int i,j;
  for(i = 1,j = 1;i <= nn;i++)
      {e = tab[j] = tab[i];
      e0 = G0.NewEdge(G.vin[e],G.vin[-e]);
      if(G0.TestPlanar() == 1)
          {n--;
          }
      else
          {j++;
          G0.DeleteEdge(e0);
          }
      }
  }
int FindNPSet(TopologicalGraph &G, svector<tedge>&tab)
  {tedge e; 
  int n=0;
  svector<int> count(0,G.ne()); count.clear();
  GraphContainer GC(G.Container());
  TopologicalGraph G0(GC);
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
  for (e=0; e<=G0.ne(); e++) initedge[e]=e;
  while (G0.TestPlanar() !=1)
      { if ((e=EraseMostMarkedEdge(G0,count))==tedge(0)) return false;
      tab[++n]=e;
      }
  ReinsertSomeEdges(G,G0,tab,n);
  return n;
  }
tedge EraseMostMarkedEdge(TopologicalGraph& G0,svector<bool> &mark,
                          svector<int>& count)
  {tedge e,e0,f0,f;
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
  GraphContainer DFSContainer;
  DFSGraph DG(DFSContainer,G0);
  DG.CotreeCritical();
  DG.Kuratowski();
  Prop<bool> keep(DG.Set(tedge()),PROP_MARK); 
  int maxcount=0;
  e0 = e = 0;
  for(tedge j = DG.ne();j > 0;j--)
      {if(!keep[j])continue;
      f0 = DG.ie(j);   // f0 belongs to G0  
      f = initedge[f0]; // f belongs to G
      ++count[f0];
      if (count[f0]>maxcount || count[f0]==maxcount && !mark[f]) 
          {maxcount = count[f0];
          e0 = f0;
          e = f;
          }
      }
  if (e0!=0) G0.DeleteEdge(e0);
  return e;
  }
int FindNPSet(TopologicalGraph &G, svector<bool> &mark, svector<tedge>&tab)
  {tedge e; 
  int n=0;
  svector<int> count(0,G.ne()); count.clear();
  GraphContainer GC(G.Container());
  TopologicalGraph G0(GC);
  Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
  for (e=0; e<=G0.ne(); e++) initedge[e]=e;
  while (G0.TestPlanar() !=1)
      { if ((e=EraseMostMarkedEdge(G0,mark,count))==tedge(0)) return false;
      tab[++n]=e;
      }
  ReinsertSomeEdges(G,G0,tab,n);
  bool newones = false;
  for (int i=1; i<=n; i++) 
      if (!mark[tab[i]])
          {mark[tab[i]]=true;
          newones=true;
          }
  return newones ? n : -n;
  }
int FindNPSet(TopologicalGraph &G)
  {svector<tedge> tab(0,G.ne()); tab.SetName("tab");
  G.Simplify();
  int m_origin = G.ne();
  G.MakeConnected();
  int n0 = FindNPSet(G,tab);
    
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  tedge e;
  for(e = 1; e <= G.ne();e++)
      {ewidth[e] = 1; ecolor[e] = Black;}
  for (int i=1; i<=n0; i++)
      {e=tab[i]; ewidth[e] = 2; ecolor[e] = Blue;}
  for(e = G.ne(); e > m_origin;e--) G.DeleteEdge(e);
  return n0;
  }

int TopologicalGraph::MaxPlanar(svector<bool>  &keep)
  {// Precondition the graph is simple
  if(!CheckSimple())return -1;
  svector<tedge>  ToBeErased(1,ne());        ToBeErased.SetName("MxPl:ToBeErased");
  svector<bool>  mark(1,ne()); mark.clear(); mark.SetName("MxPl:mark");
  svector<tedge> tab(1,ne()); tab.SetName("MxPl:tab");
  tedge e;
  int n,i;
  int n0 = ne();

  // Find a first solution
  while((n=FindNPSet(*this,mark,tab))!=0)
      {if (n<0)
          {n=-n;
          if (n>=n0) break;
          }
      if(n < n0)
          {for(i = 1;i <= n; i++)ToBeErased[i] = tab[i];
          if((n0 = n) <= 1)break;
          }
      }

  // Optimize the solution
  tedge e0,ee,einit;
  int inserted,j;
  bool ok;
  svector<bool> ReInserted(1,n0);  

  for(ee = ne();ee > 0;ee--)
      {GraphContainer GC(Container());
      TopologicalGraph G0(GC);
      Prop<tedge> initedge(G0.Set(tedge()),PROP_INITIALE);
      for(e = 1;e <= ne();e++)initedge[e] = e;
      mark.clear();
      for(i = 1;i <= n0;i++) mark[ToBeErased[i]] = true;
      G0.DeleteEdge(ee);
      ok = true;
      for(e = G0.ne();e > 0;e--)
          {einit = initedge[e];
          if(einit == ee){ok = false;break;}
          if(mark[einit])G0.DeleteEdge(e);
          }
      if(ok == false)continue;    
      // Try to add more than one edge
      ReInserted.clear();
      inserted = 0;
      for(i = 1;i <= n0;i++)
          {e = ToBeErased[i];
          e0 = G0.NewEdge(vin[e],vin[-e]);
          if(G0.TestPlanar() == 1)
              {++inserted;
              ReInserted[i] = true;
              }
          else
              G0.DeleteEdge(e0);
          }
      if(inserted <= 1)continue;
      // Solution can be optimized
      j = 0;
      for(i = 1;i <= n0;i++)
          if(ReInserted[i] == false)ToBeErased[++j] = ToBeErased[i];
      ToBeErased[++j] = ee;
      n0 = j;
      }

  for(ee = 1;ee <= ne();ee++)
      keep[ee] = true;
  for(i = 1;i <= n0;i++)
      keep[ToBeErased[i]] = false;

  return n0;
  }
int TopologicalGraph::MaxPlanar()
  {if(!CheckSimple())return -1;
  int m_origin = ne();
  MakeConnected();
  svector<bool>  keep(1,ne());
  int n0 = MaxPlanar(keep);
  if(n0 < 0)return n0;
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  tedge e;
  for(e = 1; e <= ne();e++)
      if(keep[e])
          {ewidth[e] = 1; ecolor[e] = Black;}
      else
          {ewidth[e] = 2; ecolor[e] = Blue;}
  for(e = ne(); e > m_origin;e--)DeleteEdge(e);
  return n0;
  }

