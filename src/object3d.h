/**
 *  Copyright (C) 2008 Lorenzo Bossi and Stefano Massera
 *
 *  This file is part of Fractals3D
 *
 *  Fractals3D is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 2 of the License,
 *  or (at your option) any later version.
 *
 *  Fractals3D is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *  See the GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with Fractals3D
 *  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __OBJECT3D_H
#define __OBJECT3D_H

#include "rotation.h"
#include <GL/gl.h>

class object3D{
 protected:
  int precision;
 public:
  virtual void draw(const double_point3 &){;}//un vettore che indichi la direzione di vista
  virtual void init(int n){precision=n;}//inizializza e setta una "precisione"
  virtual ~object3D(){;}
};

class pianiParalleli: public object3D{
 public:
  void draw(const double_point3 &);//un vettore che indichi la direzione di vista
};

class cubetti: public object3D{
 protected:
  static void drawPlaneXY(double semiLato, double centroX, double centroY, double centroZ);
  static void drawPlaneXZ(double semiLato, double centroX, double centroY, double centroZ);
  static void drawPlaneYZ(double semiLato, double centroX, double centroY, double centroZ);
  static void drawStar(double semiLato, double centroX, double centroY, double centroZ,const double_point3 &vista);
 public:
  void draw(const double_point3 &);//un vettore che indichi la direzione di vista
};

class pianiIntersecati: public object3D{
  QList<double_point3> punti;
  QList<double_point3> normali;
 public:
  void init(int n);
  void draw(const double_point3 &);//un vettore che indichi la direzione di vista
};

class polvere: public object3D{
 protected:
  int pointSize;
  virtual float getStep()=0;
 public:
  void draw(const double_point3 &vista);
};

class polvereRada: public polvere{
  float getStep();
 public:
  polvereRada();
};

class polvereFitta: public polvere{
  float getStep();
 public:
  polvereFitta();
};


class sfera: public object3D{
 public:
  static void drawSfera(double r,int lats=15,int longs=15);
  void draw(const double_point3 &);//un vettore che indichi la direzione di vista
};


#endif
