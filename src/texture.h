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


#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <QtGui>
#include <iostream>

#include <GL/glew.h>

#include "rotation.h"

void checkGlError(const char *msg=NULL);

// classe con tutti i mebri vuoti: semplicemente non fa nulla, verranno disegnati poligoni bianchi
class texture: public QObject{
  Q_OBJECT

 public:
  virtual ~texture(){;}
  
  virtual bool generateTexture(int, double, const double_quaternion &, bool, const double_quaternion &){
    return true;//non fa nulla, ma lo fa bene
  }
  virtual void improveTexture(int, const QVector<QColor>&){;} //imposta la taratura del grigio e i colori
  virtual void bindTexture(float *,float *,float *){;} //associa la texture passando i piani per generare le coordinate texture
  virtual bool initialize(){return true;}   //inizializza le cose che non può fare nel costruttore perché nn in ambiente opengl
  virtual void resetTexture(){;} //setta lo stato iniziale (chiamata ogni volta che cambio tipo di texture)
  virtual void unsetTexture(){;} //lo chiamo quando devo cambiare il motore prima di scaricarlo
  
  virtual bool save(QDataStream &){return false;}//non può salvare
  virtual bool load(QDataStream &){return false;}//nemmeno caricare
  
 signals:
  void progress(int);
  void error(QString);
};


class dynamicTexture: public texture{
  QString fragProg,vertProg,fragInit;
  GLuint v,f,p;
  
  QString readFile(const QString &fname);
  int printOglError(char *file, int line);
  void printInfoLog(GLuint obj);
  void printShaderInfoLog(GLuint obj);
  
  int precision;
  double zoom;
  double_quaternion pos,par;
  bool usePar;
  
 public:
  ~dynamicTexture();
  bool generateTexture(int precision, double zoom, const double_quaternion &pos, bool usePar, const double_quaternion &par);
  void improveTexture(int grey, const QVector<QColor> &color);
  void bindTexture(float *s,float *t,float *r);
  bool initialize();
  void unsetTexture();
  void resetTexture();
};


class staticTexture: public texture{
 private:
  unsigned char *texels,*texelsI;
  unsigned int texname;

  int WIDTH, HEIGHT, DEPTH, BYTES_PER_TEXEL, SIZE;

  int texel3(int s, int t, int r) {
    return ( s+ t*WIDTH + r*WIDTH * HEIGHT )*BYTES_PER_TEXEL;
  }

  void rebindTexture();
 public:
  staticTexture();
  ~staticTexture();
  bool generateTexture(int precision, double zoom, const double_quaternion &pos, bool usePar, const double_quaternion &par);
  void improveTexture(int grey, const QVector<QColor> &color);
  void bindTexture(float *s,float *t,float *r);
  bool initialize();
  void resetTexture();
  void unsetTexture();

  bool save(QDataStream &s);
  bool load(QDataStream &s);
};

#endif
