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


#include "object3d.h"

void pianiParalleli::draw(const double_point3 &vista){
  float inc=2.0/precision; // 2 è la dimensione in z da 1 a -1
  const int l=1;
  float pos=-1+inc/2;
  if (vista.z()<0){
    glBegin(GL_QUADS);
    for(int i=0;i<precision;i++,pos+=inc){
      glNormal3f(0,0,1);   //fronte
      glVertex3f(-l,-l,pos);
      glVertex3f(-l,+l,pos);
      glVertex3f(+l,+l,pos);
      glVertex3f(+l,-l,pos);
    }
    glEnd();
  }
  else{
    pos=pos+precision*inc;//for(int i=0;i<precision;i++,pos+=inc);
    pos-=inc;
    glBegin(GL_QUADS);
    for(int i=precision;i>0;i--,pos-=inc){
      glNormal3f(0,0,-1);  //retro
      glVertex3f(+l,-l,pos);
      glVertex3f(+l,+l,pos);
      glVertex3f(-l,+l,pos);
      glVertex3f(-l,-l,pos);
    }
    glEnd();
  }
}


void pianiIntersecati::init(int n){
  object3D::init(n);
  punti.clear();
  normali.clear();
  n*=4;
  static const double PI=3.14159265;
  float passo=2*PI/n;
  double_point3 punto,normale;
  for (int i=0;i<n;i++){
    punto.setX(cos(passo*i));
    punto.setY(sin(passo*i));
    punto.setZ(0);
    punti.insert(i,punto);
    normale.setX(sin(passo*i));
    normale.setY(-cos(passo*i));
    normale.setZ(0);
    normali.insert(i,normale);
  }
}
void pianiIntersecati::draw(const double_point3 &view){
  double_point3 punto,normale;
  int primo=-1,ultimo=-1;
  double coseno,max=-1,min=1;
  int tot=punti.size();
  for (int i=0;i<tot;i++){
    //quello che ha il coseno maggiore con il vettore di vista è il più distante
    punto=punti.at(i);
    punto.normalize();
    coseno=punto*view;
    if (coseno>=max){
      max=coseno;
      primo=i;
    }
    if (coseno<=min){
      min=coseno;
      ultimo=i;
    }
  }
  glBegin(GL_QUADS);
  for ( int i=(primo)%tot; i!=(ultimo)%tot; i=(i+tot-1)%tot ){
    punto=punti.at(i);
    normale=normali.at(i);
    
    glNormal3f(normale.x(),normale.y(),normale.z());
    glVertex3f(punto.x(),punto.y(),1);
    glVertex3f(punto.x(),punto.y(),-1);
    glVertex3f(0,0,-1);
    glVertex3f(0,0,1);
    
    normale = -normale;
    glNormal3f(normale.x(),normale.y(),normale.z());
    glVertex3f(0,0,1);
    glVertex3f(0,0,-1);
    glVertex3f(punto.x(),punto.y(),-1);
    glVertex3f(punto.x(),punto.y(),1);
  }
  for ( int i=(primo+1)%tot; i!=(ultimo+1)%tot; i=(i+1)%tot ){
    punto=punti.at(i);
    normale=normali.at(i);
    
    glNormal3f(normale.x(),normale.y(),normale.z());
    glVertex3f(punto.x(),punto.y(),1);
    glVertex3f(punto.x(),punto.y(),-1);
    glVertex3f(0,0,-1);
    glVertex3f(0,0,1);
    
    normale = -normale;
    glNormal3f(normale.x(),normale.y(),normale.z());
    glVertex3f(0,0,1);
    glVertex3f(0,0,-1);
    glVertex3f(punto.x(),punto.y(),-1);
    glVertex3f(punto.x(),punto.y(),1);
  }
  glEnd();
}

void polvere::draw(const double_point3 &vista){
  float passo=getStep();
  int i,j,k;
  int max=(int)(2/passo);
  glPointSize(pointSize); //ogni punto è grande 3 frammenti
  srand(50);      //voglio che sia sempre uguale...
  float i1,i2,ip;
  float j1,j2,jp;
  float k1,k2,kp;
  
  ip=jp=kp=passo;
  i1=j1=k1=-1;
  if(vista.x()>0){
    ip=-passo;
    i1-=ip*(max-1);
  }
  if(vista.y()>0){
    jp=-passo;
    j1-=jp*(max-1);
  }
  if(vista.z()>0){
    kp=-passo;
    k1-=kp*(max-1);
  }
  glBegin(GL_POINTS);
  i2=i1;
  for (i=0;i<max;i++){
    j2=j1;
    for (j=0;j<max;j++){
      k2=k1;
      for (k=0;k<max;k++){
	glNormal3f(rand()-RAND_MAX/2 , rand()-RAND_MAX/2 , rand()-RAND_MAX/2);  //tanto è attivo il rinormalizza
	glVertex3f(i2,j2,k2);
	k2+=kp;
      }
      j2+=jp;
    }
    i2+=ip;
  }
  glEnd();
  glPointSize(1); //ripristino la dimensione di default dei punti
}

float polvereRada::getStep(){
  return 0.9/(precision+5);
}

polvereRada::polvereRada(){
  pointSize=3;
}

float polvereFitta::getStep(){
  return 0.9/1.5/(precision+5);
}

polvereFitta::polvereFitta(){
  pointSize=1;
}



void sfera::drawSfera(double r,int lats,int longs){
  int i, j;
  for(i = 1; i <= lats; i++) {
    double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
    double z0  = sin(lat0);
    double zr0 =  cos(lat0);
    double lat1 = M_PI * (-0.5 + (double) i / lats);
    double z1 = sin(lat1);
    double zr1 = cos(lat1);
    glBegin(GL_QUAD_STRIP);
    for(j = 0; j <= longs; j++) {
      double lng = 2 * M_PI * (double) (j - 1) / longs;
      double x = cos(lng);
      double y = sin(lng);
      glNormal3f(x * zr0, y * zr0, z0);
      glVertex3f(x * zr0 * r, y * zr0 * r, z0 * r);
      glNormal3f(x * zr1, y * zr1, z1);
      glVertex3f(x * zr1 * r, y * zr1 * r, z1 * r);
    }
    glEnd();
  }

}

void sfera::draw(const double_point3 &){
  float inc=1.0/(precision+2);
  float r=1-inc; //raggio iniziale (più grande)
  int i;
  glCullFace(GL_FRONT);
  for(i=0;i<precision;i++,r-=inc){
    drawSfera(r,40,40);
  }
  glCullFace(GL_BACK);
  r+=inc;
  for(i=0;i<precision;i++,r+=inc){
    drawSfera(r,40,40);
  }
}

void cubetti::drawPlaneXY(double semiLato, double centroX, double centroY, double centroZ){
  glNormal3f(0,0,1);   //fronte
  glVertex3f(centroX-semiLato, centroY-semiLato, centroZ);
  glVertex3f(centroX-semiLato, centroY+semiLato, centroZ);
  glVertex3f(centroX+semiLato, centroY+semiLato, centroZ);
  glVertex3f(centroX+semiLato, centroY-semiLato, centroZ);
  
  glNormal3f(0,0,-1);   //retro
  glVertex3f(centroX+semiLato, centroY-semiLato, centroZ);
  glVertex3f(centroX+semiLato, centroY+semiLato, centroZ);
  glVertex3f(centroX-semiLato, centroY+semiLato, centroZ);
  glVertex3f(centroX-semiLato, centroY-semiLato, centroZ);
}

void cubetti::drawPlaneYZ(double semiLato, double centroX, double centroY, double centroZ){
  glNormal3f(1,0,0);   //destra
  glVertex3f(centroX, centroY-semiLato, centroZ-semiLato);
  glVertex3f(centroX, centroY-semiLato, centroZ+semiLato);
  glVertex3f(centroX, centroY+semiLato, centroZ+semiLato);
  glVertex3f(centroX, centroY+semiLato, centroZ-semiLato);
  
  glNormal3f(-1,0,0);   //sinistra
  glVertex3f(centroX, centroY+semiLato, centroZ-semiLato);
  glVertex3f(centroX, centroY+semiLato, centroZ+semiLato);
  glVertex3f(centroX, centroY-semiLato, centroZ+semiLato);
  glVertex3f(centroX, centroY-semiLato, centroZ-semiLato);
}

void cubetti::drawPlaneXZ(double semiLato, double centroX, double centroY, double centroZ){
  glNormal3f(0,1,0);   //alto
  glVertex3f(centroX+semiLato, centroY, centroZ-semiLato);
  glVertex3f(centroX+semiLato, centroY, centroZ+semiLato);
  glVertex3f(centroX-semiLato, centroY, centroZ+semiLato);
  glVertex3f(centroX-semiLato, centroY, centroZ-semiLato);
  
  glNormal3f(0,-1,0);   //basso
  glVertex3f(centroX-semiLato, centroY, centroZ-semiLato);
  glVertex3f(centroX-semiLato, centroY, centroZ+semiLato);
  glVertex3f(centroX+semiLato, centroY, centroZ+semiLato);
  glVertex3f(centroX+semiLato, centroY, centroZ-semiLato);
}

void cubetti::drawStar(double semiLato, double centroX, double centroY, double centroZ,const double_point3 &vista){
  /*
    x a destra
    y in basso
    z scente?? 

    8 casi: li faccio tutti
    anche se poi sarebbe il caso di trovare una regolartià e pulire il codice
  */
  
  if( (vista.x()> 0) && (vista.y()> 0) && (vista.z()> 0) ){
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);

    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);

    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );
    return;
  }
  if( (vista.x()> 0) && (vista.y()> 0) && (vista.z()<=0) ){ // è il primo fatto: assumiamolo giusto
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);

    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);

    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );
    return;
  }
  if( (vista.x()> 0) && (vista.y()<=0) && (vista.z()> 0) ){
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);

    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);

    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );
    return;
  }
  if( (vista.x()> 0) && (vista.y()<=0) && (vista.z()<=0) ){
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);

    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);

    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );
    return;
  }
  if( (vista.x()<=0) && (vista.y()> 0) && (vista.z()> 0) ){
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);

    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);

    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );
    return;
  }
  if( (vista.x()<=0) && (vista.y()> 0) && (vista.z()<=0) ){
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);

    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);

    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );
    return;
  }
  if( (vista.x()<=0) && (vista.y()<=0) && (vista.z()> 0) ){
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);

    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);

    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );
    return;
  }
  if( (vista.x()<=0) && (vista.y()<=0) && (vista.z()<=0) ){
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ-semiLato);
    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ-semiLato);
    drawPlaneXY(semiLato, centroX-semiLato, centroY-semiLato, centroZ         );

    //ora ho disengato il fondo: passo alla corona
    drawPlaneYZ(semiLato, centroX         , centroY-semiLato, centroZ+semiLato);
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ-semiLato);

    drawPlaneXZ(semiLato, centroX-semiLato, centroY         , centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ-semiLato);

    drawPlaneXY(semiLato, centroX+semiLato, centroY-semiLato, centroZ         );
    drawPlaneXY(semiLato, centroX-semiLato, centroY+semiLato, centroZ         );
    
    //manca solo la testa
    drawPlaneYZ(semiLato, centroX         , centroY+semiLato, centroZ+semiLato);
    drawPlaneXZ(semiLato, centroX+semiLato, centroY         , centroZ+semiLato);
    drawPlaneXY(semiLato, centroX+semiLato, centroY+semiLato, centroZ         );
    return;
  }
}


void cubetti::draw(const double_point3 &vista){
  float lato=2.0/precision; // la larghezza di una stella
  float semiLato=lato/4.0;//la larghezza di mezzo "petalo" di stella

  int i,j,k;

  float i1,i2,ip;
  float j1,j2,jp;
  float k1,k2,kp;
  
  ip=jp=kp=lato;
  i1=j1=k1=-lato/2*(precision-1);

  if(vista.x()>0){
    ip=-ip;
    i1-=ip*(precision-1);
  }
  if(vista.y()>0){
    jp=-jp;
    j1-=jp*(precision-1);
  }
  if(vista.z()>0){
    kp=-kp;
    k1-=kp*(precision-1);
  }
  glBegin(GL_QUADS);
  i2=i1;
  for (i=0;i<precision;i++){
    j2=j1;
    for (j=0;j<precision;j++){
      k2=k1;
      for (k=0;k<precision;k++){
	drawStar(semiLato, i2,j2,k2,vista);
	k2+=kp;
      }
      j2+=jp;
    }
    i2+=ip;
  }
  glEnd();
}
