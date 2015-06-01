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

#ifndef    MYQCOLORS_H
#define   MYQCOLORS_H
/*
Qt colors:
black, white, darkGray, gray, lightGray, red, green, blue, cyan, magenta, yellow, darkRed,
 darkGreen, darkBlue, darkCyan, darkMagenta, darkYellow, color0 and color1.
Pigale colors:
  White Black  Red  Green  Blue  Yellow  Violet  Orange  Cyan  Brown  Pink
  Green2  Blue2  Grey1  Grey2  Grey3  Ivory
*/
#include <TAXI/color.h>

/* static QColor color[] =  */
/*     {QColor(255,255,255),// White */
/*      QColor(0,0,0),      // Black */
/*      QColor(255,16,8),  //Red */
/*      QColor(0,255,0),    //Green */
/*      QColor(0,128,255),  //Blue */
/*      QColor(255,255,0),  //Yellow */
/*      QColor(224,102,255), //Violet */
/*      QColor(255,164,0), //Orange */
/*      QColor(0,255,255),  //Cyan */
/*      QColor(192,96,0),   //Brown */
/*      QColor(255,200,182),//Pink */
/*      QColor(120,196,164),  //Green2 */
/*      QColor(0,96,192),    //Blue2 */
/*      QColor(232,232,232),//Grey1 */
/*      QColor(164,164,164),//Grey2 */
/*      QColor(96,96,96), //Grey3 */
/*      QColor(248,238,224) //Ivory */
/*     }; */


struct Taxi_rgb {
  int red;
  int green;
  int blue;
  Taxi_rgb(int r,int g,int b): red(r), green(g), blue(b) {}
};

static Taxi_rgb _color[] = 
    {Taxi_rgb(255,255,255),// White
     Taxi_rgb(0,0,0),      // Black
     Taxi_rgb(255,16,8),  //Red
     Taxi_rgb(0,255,0),    //Green
     Taxi_rgb(0,128,255),  //Blue
     Taxi_rgb(255,255,0),  //Yellow
     Taxi_rgb(224,102,255), //Violet
     Taxi_rgb(255,164,0), //Orange
     Taxi_rgb(0,255,255),  //Cyan
     Taxi_rgb(192,96,0),   //Brown
     Taxi_rgb(255,200,182),//Pink
     Taxi_rgb(120,196,164),  //Green2
     Taxi_rgb(0,96,192),    //Blue2
     Taxi_rgb(232,232,232),//Grey1
     Taxi_rgb(164,164,164),//Grey2
     Taxi_rgb(96,96,96), //Grey3
     Taxi_rgb(248,238,224) //Ivory
    };

class ColorRepository {
 private:
  svector<Taxi_rgb> rgb;
 public:
  ColorRepository(int n,const Taxi_rgb *_rgb) : rgb(0,n-1,_rgb) {}
  ~ColorRepository() {}
  QColor operator[] (int i) const { return (i>=rgb.n())?Qt::darkRed:QColor(rgb[i].red,rgb[i].green,rgb[i].blue);}
};  

static ColorRepository color(sizeof(_color)/sizeof(struct Taxi_rgb), _color);

#ifdef COLORNAMES

static char *ColorName[] = 
    {"White",
    "Black",
    "Red",
    "Green",
    "Blue",
    "Yellow",
    "Violet",
    "Orange",
    "Cyan",
    "Brown",
    "Pink",
    "Green2",
    "Blue2",
    "Grey1",
    "Grey2",
    "Grey3",
    "Ivory"
    };
#endif
#endif
