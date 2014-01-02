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


#ifndef __ROTATION_H
#define __ROTATION_H

#include <QWidget>
#include <QResizeEvent>

#include "geometry.h"

class abstractRotation{
  double_quaternion rot;
  int dimx, dimy, posx,posy;
 public:
  abstractRotation(int x,int y);
  abstractRotation();
  void setSize(int x, int y);
  void initDrag(int x, int y);
  void drag(int x,int y);
  double_quaternion getRotation();
  void setRotation(const double_quaternion &r);
  void resetRotation();
  double_point3 transform(const double_point3 &p) const;
  void applyTransform(double_point3 &p) const;
  void applyInvTransform(double_point3 &p) const{
    double_quaternion inv(rot.getConjugate());
    p = inv * p ;
  }
  static const double_quaternion identity;
};


class rotationWidget : public QWidget{
  Q_OBJECT
    
  abstractRotation rot;
 public:
  rotationWidget(QWidget *parent = 0);
  double_quaternion getValue(){
    return rot.getRotation();
  }
 public slots:
  void setRotation(const double_quaternion &q);
 signals:
  void rotationChanged(double_quaternion q);
 protected:
  void paintEvent(QPaintEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event); 
  void resizeEvent(QResizeEvent *event){
    QSize s(event->size());
    rot.setSize(s.width(),s.height());
  }
  void changeEvent(QEvent *event){
    if ( event->type() == QEvent::EnabledChange ){
      emit update();
    }
  }
};

#endif
