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


#ifndef __GLFRACTAL_H
#define __GLFRACTAL_H

#include "texture.h"
#include "rotation.h"
#include "object3d.h"

#include <QGLWidget>
#include <QHash>
#include <stdlib.h>

#include <iostream>


class glFractal : public QGLWidget{
     Q_OBJECT

  abstractRotation rLight,rView,rTexture;

  friend QDataStream& operator<<(QDataStream &s,const glFractal &f);
  friend QDataStream& operator>>(QDataStream &s,glFractal &f);

 public:
  glFractal(QWidget *parent = 0);
  ~glFractal();
  
  QSize minimumSizeHint() const{
    return QSize(150, 150); //minima dimensione a cui posso ridurre la finestra
  }
  QSize sizeHint() const{
    return QSize(400, 400); //dimensione di partenza della finstra
  }
  
  QStringList getViewStyles();
  QStringList getEngines();
  
 public slots:
  void enableLighting(bool b);
  void setLight(const double_quaternion &q);
  void setView(const double_quaternion &q);
  void setTexture(const double_quaternion &q);

  void generateTexture(int precision,double zoom,
		       double posR,double posI,double posJ,double posK,
		       bool usePar,double parR,double parI,double parJ,double parK);
  void improveTexture(int grey, const QVector<QColor> &color);

  void setViewStyle(const QString &style);
  void setViewStyle(int precision);

  void setEngine(const QString &engine);

  void progress_slot(int n){
    emit progress(n);
  }
  void setBackground(QColor b){
    background=b;
    updateGL();
  }
 signals:
  void changedLight(double_quaternion);
  void changedView(double_quaternion);
  void changedTexture(double_quaternion);
  
  void progress(int);
  void finish();
  
 protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void setLighting(bool b);
  void resizeEvent(QResizeEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  
 private:
  texture *tex;
  object3D *obj;

  QColor background;
  double viewZoom;

  QHash<QString, object3D*> viewStyles;
  QHash<QString, texture*> engines;
  int viewPrecision;
};

#endif
