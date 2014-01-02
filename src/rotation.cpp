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


#include <QtGui>
#include "rotation.h"

//==================== rotationWidget =================================

rotationWidget::rotationWidget(QWidget *parent) : QWidget(parent){
  setMinimumSize(60,60);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}
void rotationWidget::paintEvent(QPaintEvent *){
  int side = qMin(width(), height());

  double_point3 vec(0,0,90);//centro sfera
  double_point3 front(0,0,80);
  double_point3 back(0,0,-80);
  double_point3 left(-80,0,0);
  double_point3 right(80,0,0);
  double_point3 top(0,80,0);
  double_point3 bottom(0,-80,0);

  rot.applyTransform(vec);
  rot.applyTransform(front);
  rot.applyTransform(back);
  rot.applyTransform(left);
  rot.applyTransform(right);
  rot.applyTransform(top);
  rot.applyTransform(bottom);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  //coordinate tra -100 e 100
  painter.translate(width() / 2, height() / 2);
  painter.scale(side / 200.0, side / 200.0);
  
  QColor ellipseColor(0, 0, 127, 128);
  QColor pointColor(255, 0, 0);
  QColor xColor(255, 0, 0);
  QColor yColor(0, 255, 0);
  QColor zColor(0, 0, 255);
  
  if ( !isEnabled() ){
    ellipseColor=ellipseColor.darker(300);
    pointColor=pointColor.darker(300);
    xColor=xColor.darker(300);
    yColor=yColor.darker(300);
    zColor=zColor.darker(300);
  }
  
  if (vec.z()<0){ //sul retro
    painter.setBrush(pointColor);
    painter.setPen(Qt::SolidLine);
    painter.drawEllipse(QRectF(vec.x()-10,vec.y()-10,20,20));
  }
  
  painter.setPen(Qt::NoPen);
  painter.setBrush(ellipseColor);
  painter.drawEllipse(QRectF(-80,-80,160,160));

  painter.setPen(Qt::SolidLine);
  painter.setPen(zColor);
  painter.drawLine(QLine((int)front.x(),(int)front.y(),(int)back.x(),(int)back.y()));
  painter.setPen(xColor);
  painter.drawLine(QLine((int)left.x(),(int)left.y(),(int)right.x(),(int)right.y()));
  painter.setPen(yColor);
  painter.drawLine(QLine((int)top.x(),(int)top.y(),(int)bottom.x(),(int)bottom.y()));

  if (vec.z()>=0){ //sul fronte
    painter.setBrush(pointColor);
    painter.setPen(Qt::SolidLine); 
    painter.drawEllipse(QRectF(vec.x()-10,vec.y()-10,20,20));
  }  
}
void rotationWidget::mouseDoubleClickEvent(QMouseEvent *event){
  if (event->buttons() & Qt::LeftButton) {
    rot.resetRotation();
    emit update();
    emit rotationChanged(rot.getRotation());
  }
}
void rotationWidget::mousePressEvent(QMouseEvent *event){
  if (event->buttons() & Qt::LeftButton) {
    rot.initDrag(event->pos().x(),event->pos().y());
  }
}
void rotationWidget::mouseMoveEvent(QMouseEvent *event){
  if (event->buttons() & Qt::LeftButton) {
      rot.drag(event->pos().x(),event->pos().y());
      emit update();
      emit rotationChanged(rot.getRotation());
  }
}
void rotationWidget::setRotation(const double_quaternion &q){
  rot.setRotation(q);
  emit update();
}


//==================== abstractRotation =================================


abstractRotation::abstractRotation(int x,int y):dimx(x),dimy(y){
  resetRotation();
}
abstractRotation::abstractRotation():dimx(1),dimy(1){
  resetRotation();
}
void abstractRotation::setSize(int x, int y){
  dimx=x;
  dimy=y;
}
void abstractRotation::initDrag(int x, int y){
  posx=x;
  posy=y;
}
void abstractRotation::drag(int x,int y){
  int raggio=(int)(qMin(dimx,dimy)*0.4);//prendo solo l'80% come sfera di drag e poi lo dimezzo per avere il raggio
  int xx=x-dimx/2;//coordinate inizio drag relative al centro
  int yy=y-dimy/2;
  int xx2=posx-dimx/2;//coordinate fine drag relative al centro
  int yy2=posy-dimy/2;

  double h;//altezza sulla sfera
  h=raggio*raggio-xx*xx-yy*yy;
  if (h<0) h=0;
  double_point3 v1(xx,yy,sqrt(h)); //vettore che punta all'inizio del drag
  h=raggio*raggio-xx2*xx2-yy2*yy2;
  if (h<0) h=0;
  double_point3 v2(xx2,yy2,sqrt(h)); //vettore che punta alla fine del drag
  
  double_point3 asse(v1%v2);//ottengo l'asse di rotazione come il vettore perpendicolare ai due precedenti
  asse.normalize();
  
  double seno,coseno;
  v1.normalize();
  v2.normalize();
  
  seno=(v1%v2).module(); //ottengo il seno come modulo del cross dei due vettori normalizzati
  coseno=-(v1*v2)*2; //ottengo il coseno come il dot dei due vettori normalizzati... poi lo dimezzo per avere un'approssimazione di cos(x/2)
  
  asse=asse*seno;
  double_quaternion tmp(coseno,asse.x(),asse.y(),asse.z());
  
  rot=tmp*rot;
  rot.normalize();//rinormalizzo per evitare errori di approssimazione
  //rot=normalize(rot);//rinormalizzo per evitare errori di approssimazione
  
  initDrag(x,y);
}
double_quaternion abstractRotation::getRotation(){
  return rot;
}
void abstractRotation::setRotation(const double_quaternion &r){
  rot=r;
}
void abstractRotation::resetRotation(){
  rot=identity;
}
double_point3 abstractRotation::transform(const double_point3 &p) const{
  return rot * p;
}
void abstractRotation::applyTransform(double_point3 &p) const{
  p = rot * p ;
}

const double_quaternion abstractRotation::identity(1,0,0,0);
