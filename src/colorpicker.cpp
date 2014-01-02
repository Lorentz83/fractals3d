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


#include "colorpicker.h"
#include <QColorDialog>


#include <iostream>

void colorPicker::updateColors(){
  repaint();

  QPixmap pixmap(20,20);
  pixmap.fill(color);
  editBtn->setIcon(pixmap);

  red->setValue(color.red());
  green->setValue(color.green());
  blue->setValue(color.blue());

  emit colorChanged(color);
}

colorPicker::colorPicker(QWidget *parent):QWidget(parent){
  setupUi(this);
  color=Qt::black;
  connect(red, SIGNAL( valueChanged(int) ), this, SLOT( setRed(int) ) );
  connect(green, SIGNAL( valueChanged(int) ), this, SLOT( setGreen(int) ) );
  connect(blue, SIGNAL( valueChanged(int) ), this, SLOT( setBlue(int) ) );
  connect(editBtn, SIGNAL( clicked() ), this, SLOT( displayPicker() ) );
  updateColors();
}

void colorPicker::displayPicker(){
  QColor tmpcol=QColorDialog::getColor(color, this);
  if (tmpcol.isValid())
    setColor(tmpcol);
}

void colorPicker::setRed(int r){
  if (color.red()!=r){
    color.setRed(r); 
    updateColors();
  }
}
void colorPicker::setGreen(int g){
  if(color.green()!=g){
    color.setGreen(g);
    updateColors();
  }
}
void colorPicker::setBlue(int b){
  if(color.blue()!=b){
    color.setBlue(b);
    updateColors();
  }
}

void colorPicker::setColor(QColor c){
  if (c!=color){
    color=c;
    updateColors();
  }
}

void colorPicker::paintEvent(QPaintEvent *){
  QColor color1,color2;
  
  color1=color2=color;
  color1.setRed(0);
  color2.setRed(255);
  QPalette pRed;
  QLinearGradient gRed(0,0,bRed->width(),0);
  gRed.setColorAt( 0, color1 );
  gRed.setColorAt( 1, color2 );
  pRed.setBrush(QPalette::Window,  QBrush (gRed) );
  pRed.setBrush(QPalette::Button,  QBrush (gRed) );
  bRed->setPalette(pRed);

  color1=color2=color;
  color1.setGreen(0);
  color2.setGreen(255);
  QPalette pGreen;
  QLinearGradient gGreen(0,0,bGreen->width(),0);
  gGreen.setColorAt( 0, color1 );
  gGreen.setColorAt( 1, color2 );
  pGreen.setBrush(QPalette::Window,  QBrush (gGreen) );
  pGreen.setBrush(QPalette::Button,  QBrush (gGreen) );
  bGreen->setPalette(pGreen);

  color1=color2=color;
  color1.setBlue(0);
  color2.setBlue(255);
  QPalette pBlue;
  QLinearGradient gBlue(0,0,bBlue->width(),0);
  gBlue.setColorAt( 0, color1 );
  gBlue.setColorAt( 1, color2 );
  pBlue.setBrush(QPalette::Window,  QBrush (gBlue) );
  pBlue.setBrush(QPalette::Button,  QBrush (gBlue) );
  bBlue->setPalette(pBlue);
}
