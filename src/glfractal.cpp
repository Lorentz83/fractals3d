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


#include "texture.h"
#include "glfractal.h"

#include <QtGui>
//#include <QtOpenGL>

void checkGlError();

void glFractal::resizeEvent(QResizeEvent *event){
  QSize s(event->size());
  rLight.setSize(s.width(),s.height());
  rView.setSize(s.width(),s.height());
  rTexture.setSize(s.width(),s.height());

  QGLWidget::resizeEvent(event); //passo l'evento alla classe genitore per ridimensionare correttamente la finestra opengl
}

void glFractal::mousePressEvent(QMouseEvent *event){
  rLight.initDrag(event->x(),event->y());
  rView.initDrag(event->x(),event->y());
  rTexture.initDrag(event->x(),event->y());
}

void glFractal::mouseMoveEvent(QMouseEvent *event){
  int x = event->x();
  int y = event->y();
  
  if (event->buttons() & Qt::LeftButton) {
    rView.drag(x,y);
    emit changedView(rView.getRotation());
  } else if (event->buttons() & Qt::RightButton) {
    rLight.drag(x,y);
    emit changedLight(rLight.getRotation());
  } else if (event->buttons() & Qt::MidButton) {
    rTexture.drag(x,y);
    emit changedTexture(rTexture.getRotation());
  }
  updateGL();
}

void glFractal::mouseDoubleClickEvent(QMouseEvent *event){
  if (event->buttons() & Qt::LeftButton) {
    rView.resetRotation();
    emit changedView(rView.getRotation());
  } else if (event->buttons() & Qt::RightButton) {
    rLight.resetRotation();
    emit changedLight(rLight.getRotation());
  } else if (event->buttons() & Qt::MidButton) {
    rTexture.resetRotation();
    emit changedTexture(rTexture.getRotation());
  }
  updateGL();
}

void glFractal::wheelEvent(QWheelEvent *event){
  static const double max=2.0;
  static const double min=0.1;
  static const double step=.1;
  if (event->delta()>0){
    viewZoom+=step;
    if (viewZoom>max)
      viewZoom=max;
  }
  else{
    viewZoom-=step;
    if (viewZoom<min)
      viewZoom=min;
  }
  updateGL();
}

glFractal::glFractal(QWidget *parent): QGLWidget(parent) {
  viewPrecision=1;
  viewZoom=1;

  viewStyles[QString("Parallel planes")] = new pianiParalleli();
  viewStyles[QString("Dust 2")] =  new polvereFitta();
  viewStyles[QString("Dust 1")] =  new polvereRada();
  viewStyles[QString("Circular planes")] = new pianiIntersecati();
  viewStyles[QString("Sphere")] = new sfera();
  viewStyles[QString("Cubes")] = new cubetti();

  QStringList lista=getViewStyles();
  obj=viewStyles.value(lista.at(0));
  obj->init(viewPrecision);

    
  tex = new texture();
  engines[QString("-- Dummy --")] = tex;
  engines[QString("Static")] = new staticTexture();
  engines[QString("Dynamic")] = new dynamicTexture();
}

QStringList glFractal::getViewStyles(){
  QStringList lista;
  QHashIterator<QString, object3D*> i(viewStyles);
  while (i.hasNext()) {
    i.next();
    lista<<i.key();
  }
  lista.sort();
  return lista;
}

QStringList glFractal::getEngines(){
  QStringList lista;
  QHashIterator<QString, texture*> i(engines);
  while (i.hasNext()) {
    i.next();
    lista<<i.key();
  }
  lista.sort();
  return lista;
}

void glFractal::setEngine(const QString &engine){
  tex->unsetTexture();
  disconnect (tex, SIGNAL( progress(int) ), this, SLOT( progress_slot(int) ) );
  if (engines.contains(engine))
    tex = engines.value(engine);
  connect (tex, SIGNAL( progress(int) ), this, SLOT( progress_slot(int) ) );  
  tex->resetTexture();
  emit finish();
  updateGL();
}

void glFractal::setViewStyle(const QString &style){
  if (viewStyles.contains(style))
    obj = viewStyles.value(style);
  obj->init(viewPrecision);
  updateGL();
}

void glFractal::setViewStyle(int precision){
  viewPrecision=precision;
  obj->init(viewPrecision);
  updateGL();
}

glFractal::~glFractal(){
  makeCurrent();
  if(tex)
    delete tex;
  if(obj)
    delete obj;
}

void glFractal::setLight(const double_quaternion &q){
  rLight.setRotation(q);
  updateGL();
}
void glFractal::setView(const double_quaternion &q){
  rView.setRotation(q);
  updateGL();
}
void glFractal::setTexture(const double_quaternion &q){
  rTexture.setRotation(q);
  updateGL();
}

void glFractal::initializeGL(){
  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER , 0.02); //al di sotto di questo alpha non disegno il frammento
  
  glEnable(GL_TEXTURE_3D); // enable 3d texturing
  
  glewInit();              //inizializzo le glew
  
  glEnable(GL_NORMALIZE);  //rinomalizza i vettori passati come normali
  glEnable(GL_CULL_FACE);  //
  
  //luce
  setLighting(true);
 
  //riflessi bianchi
  float tmpcol[4]={1,1,1,1};
  glMaterialfv(GL_FRONT,GL_SPECULAR,tmpcol);
  glMaterialf(GL_FRONT,GL_SHININESS,45);

  //inizializzo i motori
  QHash<QString, texture*>::iterator i;
  QString error;
  for (i=engines.begin(); i!=engines.end();) {
    if (!i.value()->initialize()){
      error+=QString(" - %1 \n").arg(i.key());
      delete i.value();
      i=engines.erase(i);
    }
    else
      i++;
  }
  if(error.length()>0){
    QMessageBox::information(this,tr("Engine"), QString(tr("Failed to initialize the following engine:\n%1they will be disabled")).arg(error) );
  }
}

void glFractal::setLighting(bool b){
  if (b){
    glEnable( GL_LIGHT0 );
    GLfloat LightAmbient[]  = { 1.2f, 1.2f, 1.2f, 1.0f }; //Ambient Light Values
    GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f }; //Diffuse Light Values
  
    glEnable(GL_LIGHTING);
    glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
  }
  else{
    glDisable(GL_LIGHTING);
    GLfloat LightAmbient[]  = { 0.0f, 0.0f, 0.0f, 0.0f }; //Ambient Light Values
    GLfloat LightDiffuse[]  = { 0.0f, 0.0f, 0.0f, 0.0f }; //Diffuse Light Values
    glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
  }
}

void glFractal::enableLighting(bool b){
  setLighting(b);
  updateGL();
}

void glFractal::paintGL() {
  glPushMatrix();
  double_point3 light(0,0,1);
  rLight.applyTransform(light);
  
  float LightPosition[4];
  LightPosition[0]=light.x();
  LightPosition[1]=light.y();
  LightPosition[2]=light.z();
  LightPosition[3]=0;
  glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );

  //glClearColor(.1,.1,.1,1); //colore di sfondo
  glClearColor(background.red()/255.0,background.green()/255.0,background.blue()/255.0,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
  double_point3 x(1,0,0);
  double_point3 y(0,1,0);
  double_point3 z(0,0,1);
  
  rView.applyTransform(x);
  rView.applyTransform(y);
  rView.applyTransform(z);
  
  float m[16];
  m[  0 ]=x.x();
  m[  1 ]=x.y();
  m[  2 ]=x.z();
  m[  3 ]=0;
  m[  4 ]=y.x();
  m[  5 ]=y.y();
  m[  6 ]=y.z();
  m[  7 ]=0;
  m[  8 ]=z.x();
  m[  9 ]=z.y();
  m[ 10 ]=z.z();
  m[ 11 ]=0;
  m[ 12 ]=0;
  m[ 13 ]=0;
  m[ 14 ]=0;
  m[ 15 ]=1;

  glLoadMatrixf(m);
  glScaled(viewZoom,viewZoom,viewZoom);

  x=double_point3(0.5,0,0);
  y=double_point3(0,0.5,0);
  z=double_point3(0,0,0.5);

  rTexture.applyTransform(x);
  rTexture.applyTransform(y);
  rTexture.applyTransform(z);

  float s[4]={0.5,   0,  0, 0.5};
  float t[4]={  0, 0.5,  0, 0.5};
  float r[4]={  0,   0,0.5, 0.5};
  
  s[0]=x.x();
  s[1]=x.y();
  s[2]=x.z();

  t[0]=y.x();
  t[1]=y.y();
  t[2]=y.z();

  r[0]=z.x();
  r[1]=z.y();
  r[2]=z.z();
  
  tex->bindTexture(r,s,t);

  double_point3 view(0,0,-1);
  rView.applyInvTransform(view);
  obj->draw( view );
  
  glPopMatrix();
}

void glFractal::resizeGL(int width, int height){
  int side = qMin(width, height);
  glViewport((width - side) / 2, (height - side) / 2, side, side);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /*
    x cresce a destra 
    y in basso
    z uscente
  */
  glOrtho(-1.5, +1.5, +1.5, -1.5, -1.5, 1.5); 
  glMatrixMode(GL_MODELVIEW);
}

void glFractal::generateTexture(int precision,double zoom,
				double posR,double posI,double posJ,double posK,
				bool usePar,double parR,double parI,double parJ,double parK){
  //connect (tex, SIGNAL( progress(int) ), this, SLOT( progress_slot(int) ) );
  tex->generateTexture(precision,zoom,double_quaternion(posR,posI,posJ,posK),usePar,double_quaternion(parR,parI,parJ,parK));
  //disconnect (tex, SIGNAL( progress(int) ), this, SLOT( progress_slot(int) ) );
  emit finish();
  updateGL();
}

void glFractal::improveTexture(int grey, const QVector<QColor> &color){
  //connect (tex, SIGNAL( progress(int) ), this, SLOT( progress_slot(int) ) );  
  tex->improveTexture(grey,color);
  //disconnect (tex, SIGNAL( progress(int) ), this, SLOT( progress_slot(int) ) );
  emit finish();
  updateGL();
}


QDataStream& operator<<(QDataStream &s,const glFractal &f){
  (f.tex)->save(s);
  return s;
}
QDataStream& operator>>(QDataStream &s,glFractal &f){
  (f.tex)->load(s);
  return s;
}
