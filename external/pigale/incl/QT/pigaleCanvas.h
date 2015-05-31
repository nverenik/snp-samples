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

#ifndef  MYCANVAS_H_INCLUDED
#define  MYCANVAS_H_INCLUDED

#include <TAXI/graphs.h> 
#include <TAXI/color.h> 
#include <TAXI/Tpoint.h> 
#include <QT/grid.h> 

#include <QPainter>
#include <q3canvas.h>
#define QCanvasPolygonalItem  Q3CanvasPolygonalItem
#define QPointArray  Q3PointArray
#define QCanvasLine Q3CanvasLine
#define QCanvasText  Q3CanvasText
#define QCanvasRectangle Q3CanvasRectangle
#define QCanvasView  Q3CanvasView
#include <Q3PointArray> 	


bool & ShowOrientation();
int  & ShowVertex();

class NodeItem;

const int line_rtti    = 7;
const int arrow_rtti = 8;
const int ntxt_rtti    = 1004;
const int node_rtti    = 1005;
const int col_rtti     = 1006;
const int edge_rtti    = 1007;
const int curs_rtti    = 1008;
const int info_rtti    = 1009;
const int thick_rtti   = 1009;

const int grid_z     =   1; // Grid
const int ntxt_z     = 128; // NodeTextItem
const int node_z     = 127; // NodeItem
const int col_z      = 130; // ColorItem
const int thick_z    = 130; // ThickItem
const int edge_z     = 126; // EdgeItem
const int arrow_z     = 125; // ArrowItem
const int curs_z     = 129; // CursItem
const int inforect_z = 131; // rect containing the info
const int info_z     = 132; // InfoItem

const double xorient = .4; 
const int sizerect   = 12;  //size and space are used to draw the color palets
const int sizerecth  = 8;   //size and space are used to draw the thick palets
const int space      = 1;   //spece + sizerect <= 13 for screens 800x600
const int BORDER     = 30;  // free space around the graph drawing 
class EdgeItem;

class ArrowItem: public QCanvasPolygonalItem 
{public:
  ArrowItem( EdgeItem *edgeitem,GraphWidgetPrivate* g);
  ~ArrowItem();
  void ComputeCoord();
  void SetColor();
  void SetColor(QColor col);
  int rtti() const;
 private:
  QPointArray areaPoints () const;
  void drawShape ( QPainter & p );
  GraphWidgetPrivate* gwp;
  EdgeItem *edgeItem;
  QPointArray pts,refresh;
};

class EdgeItem: public QCanvasLine
{public:
  EdgeItem(tedge &ee,GraphWidgetPrivate* g);
  EdgeItem(tedge &ee,int x_from,int y_from,int x_to,int y_to,bool l
	   ,GraphWidgetPrivate* g);
  void SetColor(QColor c,bool both=true);
  void SetColors(QColor c1, QColor c2);
  void setFromPoint(int x,int y) ;
  void setToPoint(int x,int y);
  int rtti() const;

  tedge e;
  EdgeItem *opp;
  ArrowItem *arrow;
  bool lower;
private:
  GraphWidgetPrivate* gwp;
};

class CursItem: public QCanvasLine
{public:
  CursItem(tvertex &v,QPoint &p,GraphWidgetPrivate* g);
  void setToPoint(int x,int y);
  int rtti() const;
  tvertex v;
  private:
  GraphWidgetPrivate* gwp;
};

class InfoItem: public QCanvasText
{public:
  InfoItem(GraphWidgetPrivate* g,QString &t,QPoint &p);
  int rtti() const;

  QCanvasRectangle* rectitem;
  private:
  GraphWidgetPrivate* gwp;
};

class NodeTextItem: public QCanvasText
{public:
  NodeTextItem(GraphWidgetPrivate* g,QString &t);
  ~NodeTextItem() {}
  void SetColor(QColor c);
  void SetText(QString t){setText(t);update();}
  int rtti() const;

  NodeItem *nodeitem;
private:
  GraphWidgetPrivate* gwp;
};

class ColorItem: public QCanvasRectangle
{public:
  ColorItem(GraphWidgetPrivate* g,QRect &rect,int pen_color,int brush_color,bool node);
  ~ColorItem() {}
  int rtti() const;
  void SetPenColor(int pen_color);
  int brush_color;
  bool node;
private:
  GraphWidgetPrivate* gwp;
};

class ThickItem: public QCanvasRectangle
{public:
  ThickItem(GraphWidgetPrivate* g,QRect &rect,int ewidth,int brush_color);
  ~ThickItem() {}
  int rtti() const;
  void SetBrushColor(int bcolor);
  int brush_color;
  int width;
private:
  GraphWidgetPrivate* gwp;
};

class NodeItem: public QCanvasRectangle
{public:
  NodeItem(tvertex &vv,GraphWidgetPrivate* g,QRect &rect);
  ~NodeItem() {}
  void moveBy(double dx, double dy);
  void moveTo(Tpoint &p,double eps = .0);
  void SetColor(QColor c);
  int rtti() const;

  int width;
  int height;
  tvertex v;
  NodeTextItem *nodetextitem;
private:
  GraphWidgetPrivate* gwp;
};

class GraphEditor : public QCanvasView
{public:
  GraphEditor(GraphWidgetPrivate *g,QWidget* parent=0,const char* name=0,Qt::WFlags f=0);
  ~GraphEditor();
  QSize sizeHint() const;
  double zoom;
  void initialize();
  void clear();
  void load(bool initgrid = true);
  void print(QPrinter* printer);
  void png(int size);
  void Normalise();
  bool InitGrid(Tgrid &g);
  void UndoGrid();
  Tgrid ParamGrid(int nx);
  void DrawGrid(const Tgrid &g);
  void showGrid(bool show);
  void PrintSizeGrid();
  void clearGrid();
  void ToGrid(tvertex &v);
  void ToGrid(QPoint &p);
  void Zoom(int dir);
  void Spring();
  void SpringPreservingMap(bool draw=true);
  int SpringJacquard();
  int FindItem(QPoint &p,NodeItem* &node,EdgeItem* &edge);
  int FindItem(QPoint &p,EdgeItem* &edge);
  int FindItem(QPoint &p,ColorItem* &coloritem);
  int FindItem(QPoint &p,ThickItem* &thickitem);
//   void EraseColorVertices();
//   void EraseColorEdges();
//   void EraseThickEdges();
  int nxstep,nystep;
  bool GridDrawn;
  Tgrid current_grid, graph_grid,old_grid,undo_grid;

private:
  void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseReleaseEvent(QMouseEvent*);
  void contentsMouseMoveEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent *event);
  void showEvent(QShowEvent*);
  void hideEvent(QHideEvent*);
  void resizeEvent(QResizeEvent*);
  void paintEvent(QPaintEvent *);
  void UpdateSizeGrid();
  void keyPressEvent(QKeyEvent *k);

  QPoint start_position;
  GraphWidgetPrivate* gwp;
  bool DoNormalise;
  bool is_init;
  short color_node;
  short color_edge;
  int width_edge;
  double xepsilon,yepsilon;
  double min_used_x,max_used_x;
  double min_used_y,max_used_y;
  double xminstep,yminstep;
  double x_min,x_max,y_min,y_max;
  double xstep,ystep;
  double undoxstep,undoystep;
  bool IsGrid;
  int key_pressed;
};


NodeItem* CreateNodeItem(tvertex &v,GraphWidgetPrivate* g);
void  CreateColorItems(GraphWidgetPrivate* g,int color_node,int color_edge);
void  CreateThickItems(GraphWidgetPrivate* g,int width_edge);
InfoItem* CreateInfoItem(GraphWidgetPrivate* g,QString &t,QPoint &p);
EdgeItem* CreateEdgeItem(tedge &e,GraphWidgetPrivate* g);
void CreatePenBrush();
#endif
