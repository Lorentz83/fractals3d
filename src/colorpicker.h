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


#ifndef __COLORPICKER_H
#define __COLORPICKER_H

#include "ui_colorpicker.h"
#include <QMainWindow>


class colorPicker: public QWidget, private Ui_colorPicker{
Q_OBJECT

  QColor color;
  void updateColors();
  
protected:
  void paintEvent(QPaintEvent *);
public:
  colorPicker(QWidget *parent = 0);
public slots:
  void setRed(int r);
  void setGreen(int g);
  void setBlue(int b);
  void setColor(QColor c);
  void displayPicker();
signals:
  void colorChanged(QColor);
};

#endif
