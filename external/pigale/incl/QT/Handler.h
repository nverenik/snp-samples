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

#ifndef HANDLER_H
#define HANDLER_H

#include <Pigale.h> 

bool & SchnyderRect();
bool & SchnyderLongestFace();
bool & SchnyderColor();
int EmbedHandler(GraphContainer &GC,int action,int &drawing); 
int OrientHandler(GraphContainer &GC,int action); 
int AlgoHandler(GraphContainer &GC,int action); 
int DualHandler(GraphContainer &GC,int action); 
int RemoveHandler(GraphContainer &GC,int action); 
int AugmentHandler(GraphContainer &GC,int action); 
int AlgoHandler(GraphContainer &GC,int action,int nn); 
int GenerateHandler(GraphContainer &GC,int action,int n1_gen,int n2_gen,int m_gen);
// in Generate.cpp
bool & EraseMultipleEdges();
#endif
