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


#ifndef __WINDOW_H
#define __WINDOW_H

#include "glfractal.h"
#include "rotation.h"
#include "colorpicker.h"
#include "ui_mainwindow.h"
#include <QtGui>

class datiFrattale{
 public:
   datiFrattale():nome("Default"){
    c=false;
    cR=cI=cJ=cK=0;
    posR=posI=posJ=posK=0;
    zoom=1;
  }
  QString nome;
  double posR,posI,posJ,posK;
  double zoom;
  bool c;
  double cR,cI,cJ,cK;
};

QTextStream& operator>>(QTextStream& ,datiFrattale& );



class mainWindow: public QMainWindow, private Ui::MainWindow{
  Q_OBJECT

  QString dir;// ricordo la directory di salvataggio/lettura

public:
  static const QString colori;
  static const QString frattaliDef;

  mainWindow();
  ~mainWindow();

 private slots:
  void valioriDefault(const QString &);
  void aboutWindow();
  void aboutQt();

 protected:
  std::vector<datiFrattale> valoriFrattali;

  void closeEvent(QCloseEvent*);

 public slots:
  void finishMsg(){
    setStatusBar(tr("Finish"),1000);
    resetProgress();
  }
  void setProgress(int n){
    progressBar->setValue(n);
    setCursor(Qt::WaitCursor);
  }
  void resetProgress(){
    progressBar->reset();
    setCursor(Qt::ArrowCursor);
  }
  void setStatusBar(const QString &c,int time){
    statusbar->showMessage(c,time);
  }
  void resetStatusBar(){
    statusbar->clearMessage();
  }
  void btnApplicaFractal();
  void btnGenerateFractal();
  void saveTexture();
  void loadTexture();
  void saveImage();
  void saveFractal();
 signals:
  void generateFractal(int precision,double zoom, double posR,double posI,double posJ,double posK, bool usePar,double parR,double parI,double parJ,double parK);
  void improveFractal(int, QVector<QColor>);
};


#endif
