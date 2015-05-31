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

#ifndef  GRAPHWIDGETPRIVATE_H_INCLUDED
#define  GRAPHWIDGETPRIVATE_H_INCLUDED

#if QT_VERSION >= 0x40000

#define QCanvas Q3Canvas
#endif 
class GraphWidgetPrivate
{public:
  GraphWidgetPrivate()
      {is_init = 0;
      pGG = 0;
      canvas = 0;
      editor = 0;
      SizeGrid = 100;
      ShowGrid = false;
      CanvasHidden = false;
      }
  ~GraphWidgetPrivate()
      {delete canvas;
      }
  bool is_init;
  bool ShowGrid;
  bool FitToGrid;
  bool ForceGrid;
  bool ForceGridOk;
  bool OldFitToGrid;
  bool CanvasHidden;
  bool RedrawGrid;
  int FitSizeGrid ;
  int OldFitSizeGrid ;
  int SizeGrid ;
  QCanvas* canvas;
  NodeItem* moving_item;
  CursItem* curs_item;
  InfoItem* info_item;
  GeometricGraph* pGG;
  GraphEditor* editor;
  pigaleWindow *mywindow;
  bool moving_subgraph;
  GraphWidget *GW;
  svector<ColorItem *>NodeColorItem;
  svector<ColorItem *>EdgeColorItem;
  svector<ThickItem *>EdgeThickItem;
  int fontsize;
};

#endif
