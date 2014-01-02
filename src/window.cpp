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


#include "window.h"


const QString mainWindow::colori("colori");
const QString mainWindow::frattaliDef("frattali");

mainWindow::mainWindow():dir( QDir::homePath() ){
  setupUi(this);
  resetProgress();
  
  //aggiungo il widget di opengl

  glf->updateGL();//forzo un disegno così da sapere quali motori vanno disattivati
  
  //menu
  connect(action_Esci, SIGNAL(triggered()), this, SLOT( close() ));
  connect(action_About, SIGNAL(triggered()), this, SLOT( aboutWindow() ));
  connect(actionAbout_Qt, SIGNAL(triggered()), this, SLOT( aboutQt() ));
  connect(action_Salva_immagine, SIGNAL(triggered()), this, SLOT( saveImage() ));
  connect(action_Save_fractal, SIGNAL(triggered()), this, SLOT( saveFractal() ));

  connect(action_Save_texture, SIGNAL(triggered()), this, SLOT( saveTexture() ) );
  connect(action_Load_texture, SIGNAL(triggered()), this, SLOT( loadTexture() ) );

  //combo
  connect( frattaliDefault, SIGNAL( currentIndexChanged (const QString&) ), this, SLOT( valioriDefault(const QString&) ) );

  //riempio combo box colori
  QDir dirColori(QCoreApplication::applicationDirPath()+QDir::separator()+colori);
  if (dirColori.exists()){
    dirColori.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dirColori.setSorting(QDir::Name);
    QFileInfoList list = dirColori.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      if (fileInfo.suffix()=="png"){
	QPixmap icona(fileInfo.filePath());
	aggColoreSel->addItem(QIcon(icona.scaled(22,22)),fileInfo.baseName());
      }
    }
  }
 
  //riempio combo box frattali
  QDir dirFrattali(QCoreApplication::applicationDirPath()+QDir::separator()+frattaliDef);
  if (dirFrattali.exists()){
    dirFrattali.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dirFrattali.setSorting(QDir::Name);
    QFileInfoList list=dirFrattali.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      if (fileInfo.suffix()=="frt"){
	QFile data(fileInfo.filePath());
	if ( data.open(QIODevice::ReadOnly|QIODevice::Text) ) {
	  QTextStream in(&data);
	  datiFrattale f;
	  in>>f;
	  f.nome=fileInfo.baseName();
	  frattaliDefault->addItem(f.nome);
	  valoriFrattali.push_back(f);
	  data.close();
	}
      }
    }
  }
  
  //combo box e spin visualizzazione 
  viewStyle->insertItems(0,glf->getViewStyles());

  connect(viewStyle, SIGNAL( currentIndexChanged(QString) ), glf, SLOT( setViewStyle(const QString&) ) );
  connect(viewPrecision, SIGNAL( valueChanged(int) ), glf, SLOT( setViewStyle(int) ) );

  //combo box engine
  engine->insertItems(0,glf->getEngines());
  connect(engine, SIGNAL( currentIndexChanged(QString) ), glf, SLOT( setEngine(const QString&) ) );
  
  //segnali rotazione da opengl a me
  connect(glf, SIGNAL(changedLight(double_quaternion)), lightPos, SLOT( setRotation(const double_quaternion &) ));
  connect(glf, SIGNAL(changedView(double_quaternion)), viewPos, SLOT( setRotation(const double_quaternion &) ));
  connect(glf, SIGNAL(changedTexture(double_quaternion)), texturePos, SLOT( setRotation(const double_quaternion &) ));
  
  //da me a opengl
  connect(lightPos, SIGNAL( rotationChanged(double_quaternion) ), glf, SLOT( setLight(const double_quaternion &) ));
  connect(viewPos, SIGNAL( rotationChanged(double_quaternion) ), glf, SLOT( setView(const double_quaternion &) ));
  connect(texturePos, SIGNAL( rotationChanged(double_quaternion) ), glf, SLOT( setTexture(const double_quaternion &) ));
  
  //pulsanti vari
  connect(changeLightPos, SIGNAL( toggled(bool) ), glf, SLOT( enableLighting(bool) ) );
  connect(frattaliOk, SIGNAL( clicked() ), this, SLOT( btnGenerateFractal() ) );

  connect(this, SIGNAL( generateFractal(int,double,double,double,double,double,bool,double,double,double,double) ), 
	  glf, SLOT( generateTexture(int,double,double,double,double,double,bool,double,double,double,double) ) );
  
  connect(aggApplica, SIGNAL( clicked() ), this, SLOT( btnApplicaFractal() ) );
  connect(this, SIGNAL( improveFractal(int,QVector<QColor>) ), glf, SLOT( improveTexture(int, const QVector<QColor>&) ) );

  connect(backgroundColor, SIGNAL( colorChanged(QColor) ), glf, SLOT( setBackground(QColor) ) );

  connect(glf, SIGNAL( finish() ), this , SLOT( finishMsg() ));
  connect(glf, SIGNAL( progress(int) ), this , SLOT( setProgress(int) ));

}

mainWindow::~mainWindow(){
}

void mainWindow::valioriDefault(const QString &nome){
  for (unsigned int i=0;i<valoriFrattali.size();i++){
    if (nome==valoriFrattali[i].nome){
      positionR->setValue(valoriFrattali[i].posR);
      positionI->setValue(valoriFrattali[i].posI);
      positionJ->setValue(valoriFrattali[i].posJ);
      positionK->setValue(valoriFrattali[i].posK);
      zoomFactor->setValue(valoriFrattali[i].zoom);
      frattaliC->setChecked(valoriFrattali[i].c);
      frattaliCR->setValue(valoriFrattali[i].cR);
      frattaliCI->setValue(valoriFrattali[i].cI);
      frattaliCJ->setValue(valoriFrattali[i].cJ);
      frattaliCK->setValue(valoriFrattali[i].cK);
      return;
    }
  }
  //default
  positionR->setValue(.0);
  positionI->setValue(.0);
  positionJ->setValue(.0);
  positionK->setValue(.0);
  zoomFactor->setValue(.0);
  frattaliC->setChecked(false);
  frattaliCR->setValue(.0);
  frattaliCI->setValue(.0);
  frattaliCJ->setValue(.0);
  frattaliCK->setValue(.0);
}

void mainWindow::aboutWindow(){
  QMessageBox::about(this, tr("About Fractals3D"),
		     tr("<p><b>Fractals3D</b>, Copyright (C) 2008 Lorenzo Bossi and Stefano Massera</p>"
			"<p>Fractals3D is free software: you can redistribute it and/or modify"
			"it under the terms of the GNU General Public License as published"
			"by the Free Software Foundation, either version 2 of the License,"
			"or (at your option) any later version.</p>"
			"<p>Fractals3D is distributed in the hope that it will be useful, but"
			"WITHOUT ANY WARRANTY; without even the implied warranty of"
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
			"See the <a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GNU General Public License</a> for more details.</p>"
			));
}

void mainWindow::aboutQt(){
  QMessageBox::aboutQt(this, tr("About Qt"));
}
void mainWindow::closeEvent(QCloseEvent* e){
  // if (QMessageBox::question(this," ",tr("Are you sure?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes)
  //   QWidget::closeEvent(e);
  // else
  //   e->ignore();
}
void mainWindow::saveImage(){
  QString path;

  QFileDialog dialog(this);
  QStringList filters;

  QStringList exts;
  exts<<"png"<<"jpg"<<"bmp";

  for (int i=0;i<exts.size();i++){
    filters<<tr("%1 image").arg(exts.at(i).toUpper())+" (*."+exts.at(i)+")";
  }

  dialog.setWindowTitle(tr("Save image as..."));
  dialog.setFilters(filters);
  dialog.setDirectory(dir);
  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  if (dialog.exec()){
    dir=dialog.directory().path(); //ricordo l'ultima directory di salvataggio

    path=dialog.selectedFiles().first();
    QString filtro=dialog.selectedFilter();

    QString ext(".png");
    for (int i=0;i<exts.size();i++){
      if (filtro.endsWith( "(*."+exts.at(i)+")" ) ){
        ext=exts.at(i);
        break;
      }
    }
    ext="."+ext;
    if ( !path.endsWith(ext) )
      path+=ext;
    glf->makeCurrent();
    QImage image(glf->grabFrameBuffer());
    if (!image.save(path))
      QMessageBox::critical(this,tr("Error"),tr("Can't save the image,\ncontrol write permission"));
  }

}

void mainWindow::saveFractal(){
  QString path;
  QFileDialog dialog(this);
  QString ext(".frt");
  dialog.setWindowTitle(tr("Save fractal as..."));
  dialog.setFilter(tr("Fractal (*.frt)"));
  dialog.setDirectory(QCoreApplication::applicationDirPath()+QDir::separator()+frattaliDef);

  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  if (dialog.exec()){
    path=dialog.selectedFiles().first();
    if ( !path.endsWith(ext) )
      path+=ext;
    
    QFile data(path);
    if ( data.open(QIODevice::WriteOnly|QIODevice::Text) ){
      QTextStream out(&data);
      out<<positionR->value()<<'\n'<<positionI->value()<<'\n'
	 <<positionJ->value()<<'\n'<<positionK->value()<<'\n'
	 <<zoomFactor->value()<<'\n'
	 <<(frattaliC->isChecked()?"true":"false")<<'\n'
	 <<frattaliCR->value()<<'\n'<<frattaliCI->value()<<'\n'
	 <<frattaliCJ->value()<<'\n'<<frattaliCK->value()<<'\n';
      data.close();
    }
    else
      QMessageBox::critical(this,tr("Error"),tr("Can't save the image,\ncontrol write permission"));
  }

}

void mainWindow::saveTexture(){
  QString path;

  QFileDialog dialog(this);
  QString ext(".tex");
  dialog.setWindowTitle(tr("Save texture as..."));
  dialog.setFilter(tr("Texture (*.tex)"));
  dialog.setDirectory(dir);

  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  if (dialog.exec()){
    dir=dialog.directory().path(); //ricordo l'ultima directory di salvataggio
    
    path=dialog.selectedFiles().first();
    if ( !path.endsWith(ext) )
      path+=ext;
    
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file); 
    out<<*glf;
  }
}
void mainWindow::loadTexture(){
  QString path;

  QFileDialog dialog(this);
  QString ext(".tex");
  dialog.setWindowTitle(tr("Load texture..."));
  dialog.setFilter(tr("Texture (*.tex)"));
  dialog.setDirectory(dir);

  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);

  if (dialog.exec()){
    dir=dialog.directory().path(); //ricordo l'ultima directory di salvataggio
    
    path=dialog.selectedFiles().first();

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file); 
    in>>*glf;
  }

}

void mainWindow::btnApplicaFractal(){
  int grey;
  QVector<QColor> color;
  if ( aggCorrezioneGrigi->isChecked() )
    grey=aggCorrezioneGrigiVal->value();
  else
    grey=-1;
  if ( aggColore->isChecked() ){
    QImage img( QCoreApplication::applicationDirPath()+QDir::separator()+colori+QDir::separator()+aggColoreSel->currentText() );
    QImage alpha( img.alphaChannel () );//la funzione pixel non prende l'alpha, quindi lo carico a mano
    for (int i=0;i<256;i++){
      QColor col(img.pixel(i,1));
      col.setAlpha(qGray(alpha.pixel(i,1)));
      color.append( col );
    }
  }
  emit improveFractal(grey, color);
}

void mainWindow::btnGenerateFractal(){
  emit generateFractal(precision->value(),zoomFactor->value(),
		       positionR->value(),positionI->value(),positionJ->value(),positionK->value(),
		       frattaliC->isChecked(),frattaliCR->value(),frattaliCI->value(),frattaliCJ->value(),frattaliCK->value() );
}

QTextStream& operator>>(QTextStream& in,datiFrattale& f){
  QString b;
  in>>f.posR;
  in>>f.posI;
  in>>f.posJ;
  in>>f.posK;
  in>>f.zoom;
  in>>b;
  if (b.compare("true")==0)
    f.c=true;
  else
    f.c=false;
  in>>f.cR;
  in>>f.cI;
  in>>f.cJ;
  in>>f.cK;
  return in;
}


int main(int argc, char *argv[]){
  QApplication app(argc, argv);
  
  QPixmap pixmap(":/resource/splash.png");
  QSplashScreen splash(pixmap);
  splash.setMask(pixmap.mask());
  splash.show();
  app.processEvents();

#if defined(_WIN32)
  //se sono in windows visto che non ho una console redirigo l'output su un file di log
  QString fname(QCoreApplication::applicationDirPath()+QDir::separator()+"output.log");
  freopen(fname.toAscii().data(), "w", stdout);
  freopen(fname.toAscii().data(), "a", stderr);
#endif  
  
  QString lang(QLocale::system().name());

  QTextStream out(stdout);
  out<<"Loading localization "<<lang<<"\n";

  QTranslator translator;
  translator.load("qt_"+lang);
  app.installTranslator(&translator);

  mainWindow window;
  window.show();
  splash.finish(&window);

  return app.exec();
}
