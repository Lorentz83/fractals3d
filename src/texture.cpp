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

QString dynamicTexture::readFile(const QString &fname){
  QFile data(fname);
  if ( data.open(QIODevice::ReadOnly) ) {
    QTextStream in(&data);
    return in.readAll();
  }
  return QString("");
}

int dynamicTexture::printOglError(char *file, int line){
  // Returns 1 if an OpenGL error occurred, 0 otherwise.
  GLenum glErr;
  int    retCode = 0;
  
  glErr = glGetError();
  while (glErr != GL_NO_ERROR){
    //printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
    printf("glError in file %s @ line %d: %d\n", file, line, glErr);
    retCode = 1;
    glErr = glGetError();
  }
  return retCode;
}

void dynamicTexture::printInfoLog(GLuint obj){
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;
  glGetProgramiv(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,&infologLength);
  if (infologLength > 0){
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}

void dynamicTexture::printShaderInfoLog(GLuint obj){
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0){
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void dynamicTexture::bindTexture(float *s,float *t,float *r){
  GLuint locX,locY,locZ;
  locX = glGetUniformLocation(p,"textureRotationX");
  glUniform4fv(locX,1,t);
  locY = glGetUniformLocation(p,"textureRotationY");
  glUniform4fv(locY,1,r);
  locZ = glGetUniformLocation(p,"textureRotationZ");
  glUniform4fv(locZ,1,s);
}

void dynamicTexture::improveTexture(int grey, const QVector<QColor> &color){
  emit progress(0);	
  glDetachShader(p,f);
  bool appG=grey!=-1;
  bool appC=color.size()>=255;
  QString fragComplete(fragProg);

  QString parProg;
  if (usePar){
    parProg=QString("c=vec4(%1,%2,%3,%4);")
      .arg(par.R_component_1(),0,'f',5)
      .arg(par.R_component_2(),0,'f',5)
      .arg(par.R_component_3(),0,'f',5)
      .arg(par.R_component_4(),0,'f',5);
  }
  else{
    parProg="c=quaternion;";
  }
  fragComplete.replace("/*%par%*/",parProg);
  
  QString init;
  double max_s=(pos.R_component_1()+1)/zoom;
  double min_s=(pos.R_component_1()-1)/zoom;
  double max_t=(pos.R_component_2()+1)/zoom;
  double min_t=(pos.R_component_2()-1)/zoom;
  double max_r=(pos.R_component_3()+1)/zoom;
  double min_r=(pos.R_component_3()-1)/zoom;

  init+=QString("quaternion[0]=rotatedVertex.z*%1+%2;\n").arg((max_s-min_s)/2.0,0,'f',5).arg(pos.R_component_1(),0,'f',5);
  init+=QString("quaternion[1]=rotatedVertex.x*%1+%2;\n").arg((max_t-min_t)/2.0,0,'f',5).arg(pos.R_component_2(),0,'f',5);
  init+=QString("quaternion[2]=rotatedVertex.y*%1+%2;\n").arg((max_r-min_r)/2.0,0,'f',5).arg(pos.R_component_3(),0,'f',5);
  init+=QString("quaternion[3]=%1;\n").arg(pos.R_component_4(),0,f,5);
  fragComplete.replace("/*%init%*/",init);
  
  QString expandFor;
  int iteratelimit = precision*6; 
  //const double modulelimit = 4;//20;
  double r,g,b,a;
  double aa=100-(grey*9.0/10);
  double bb=255.0/(exp(255.0/aa)-1);
  for (int i=0;i<iteratelimit;i++){
    int colore=255-lround((double)255*((double)i/(double)iteratelimit));
    if(appG){
      colore=(int) (bb*(exp( (colore) /aa)-1));
    }
    r=g=b=colore;
    a=255-colore;
    if(appC){
      QColor col(color[colore]);
      r=col.red();
      g=col.green();
      b=col.blue();
      a=col.alpha();      
    }
    r/=255;g/=255;b/=255;a/=255;
    expandFor +=QString("if (absolute(quaternion)<4.0){\n\
	quaternion=multquat(quaternion,quaternion);quaternion=quaternion+c;color= vec4(%1,%2,%3,%4);\n")
      .arg(r,0,'f',5)
      .arg(g,0,'f',5)
      .arg(b,0,'f',5)
      .arg(a,0,'f',5); 
  }
  for (int i=0;i<iteratelimit;i++){
    expandFor +="}";
  }
  fragComplete.replace("/*%for%*/",expandFor);

  QByteArray af(fragComplete.toAscii());
  const char *ff = af.constData() ;
  
  glShaderSource(f, 1, &ff ,NULL);
  glCompileShader(f);
  emit progress(40);
  printShaderInfoLog(f);
  glAttachShader(p,f);
  emit progress(60);
  glLinkProgram(p);
  printInfoLog(p);
  emit progress(70);
  glUseProgram(p);
  emit progress(100);
}
bool dynamicTexture::generateTexture(int precision, double zoom, const double_quaternion &pos, bool usePar, const double_quaternion &par){
  this->precision=precision;
  this->zoom=zoom;
  this->pos=pos;
  this->par=par;
  this->usePar=usePar;
  improveTexture(-1, QVector<QColor>() ); //chiamo la improve con i parametri che dicono di non fare nulla...
  return true;
}

void dynamicTexture::unsetTexture(){
  glDetachShader(p,f);
  glDetachShader(p,v);
  glUseProgram(0);
  checkGlError("dynamicTexture::unsetTexture() - glUseProgram(0);");
}

void dynamicTexture::resetTexture(){
  emit progress(0);
  QByteArray av(vertProg.toAscii());
  const char *vv = av.constData() ;
  glShaderSource(v, 1, &vv ,NULL);
  glCompileShader(v);
  checkGlError("dynamicTexture::resetTexture() - glCompileShader(v)");
  printShaderInfoLog(v);
  glAttachShader(p,v);
  checkGlError("dynamicTexture::resetTexture() - glAttachShader(p,v)");
  emit progress(30);
  
  QByteArray af(fragInit.toAscii());
  const char *ff = af.constData() ;
  glShaderSource(f, 1, &ff ,NULL);
  glCompileShader(f);
  checkGlError("dynamicTexture::resetTexture() - glCompileShader(f)");
  printShaderInfoLog(f);
  glAttachShader(p,f);
  checkGlError("dynamicTexture::resetTexture() - glAttachShader(p,f)");
  emit progress(60);
  
  glLinkProgram(p);
  checkGlError("dynamicTexture::resetTexture() - glLinkProgram(p)");
  printInfoLog(p);
  glUseProgram(p);
  checkGlError("dynamicTexture::resetTexture() - glUseProgram(p)");
  emit progress(100);
}

bool dynamicTexture::initialize(){
  if (!glCreateProgram)
    return false;
  /*
  vertProg=readFile(QCoreApplication::applicationDirPath()+QDir::separator()+"shaderprog"+QDir::separator()+"prog.vert");
  fragProg=readFile(QCoreApplication::applicationDirPath()+QDir::separator()+"shaderprog"+QDir::separator()+"prog.frag");
  fragInit=readFile(QCoreApplication::applicationDirPath()+QDir::separator()+"shaderprog"+QDir::separator()+"init.frag");
  */
  //linko i file nell'eseguibile col meccanismo delle risorse di qt
  vertProg=readFile(":/shaderprog/prog.vert");
  fragProg=readFile(":/shaderprog/prog.frag");
  fragInit=readFile(":/shaderprog/init.frag");
   
  p = glCreateProgram();
  checkGlError("dynamicTexture::initialize() - glCreateProgram()");
  v = glCreateShader(GL_VERTEX_SHADER);
  checkGlError("dynamicTexture::initialize() - glCreateShader(GL_VERTEX_SHADER)");
  f = glCreateShader(GL_FRAGMENT_SHADER);
  checkGlError("dynamicTexture::initialize() - glCreateShader(GL_FRAGMENT_SHADER)");
  return true;
}
dynamicTexture::~dynamicTexture(){
  //potrebbe non essere attivo il supporto per la programmazione della sk video
  if(glDeleteShader && glDeleteProgram){
    glDeleteShader(v);
    glDeleteShader(f);
    glDeleteProgram(p);
  }
}
//===========================================================================================================

staticTexture::staticTexture():texels(NULL),texelsI(NULL),texname(0){

}

staticTexture::~staticTexture(){
  if(texels)
    free(texels);
  if(texelsI)
    free(texelsI);
}

bool staticTexture::initialize(){
  if (!glTexImage3D)
    return false;
  
  glGenTextures(1, &texname);
  return true;
}
void staticTexture::resetTexture(){
  WIDTH=HEIGHT=DEPTH=4;
  BYTES_PER_TEXEL=4;
  SIZE=WIDTH * HEIGHT * DEPTH * BYTES_PER_TEXEL;

  if (texelsI)
    free(texelsI);
  texelsI=NULL;
  if (texels)
    free(texels);
  texels = (unsigned char *)malloc(SIZE);
  for (int c=0; c<DEPTH; c++){  
    for (int b=0; b<HEIGHT; b++){
      for (int a=0; a<WIDTH; a++){
	texels[ texel3(a, b, c) + 0 ] = 0xff*a/WIDTH;
	texels[ texel3(a, b, c) + 1 ] = 0xff*b/HEIGHT;
	texels[ texel3(a, b, c) + 2 ] = 0xff*c/DEPTH;
	texels[ texel3(a, b, c) + 3 ] = 0xff;
      }
    }
  }

  glBindTexture(GL_TEXTURE_3D, texname);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
  
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);  
  
  checkGlError("staticTexture::resetTexture() - glTexImage3D");
}

void staticTexture::unsetTexture(){
  glDeleteTextures(1,&texname);
  if (texelsI)
    free(texelsI);
  texelsI=NULL;
  if (texels)
    free(texels);  
  texels=NULL;
}

bool staticTexture::generateTexture(int precision, double zoom, const double_quaternion &pos, bool usePar, const double_quaternion &par){
  //spazio texture: [0, 1]
  //non-power-of-two textures are not supported and the width, height, or depth cannot be represented as 2^k + 2 (border) for some integer value of k.
  //128 32  64

  int DIM=1;
  for (int n=0;n<precision;n++)
    DIM*=2;
  
  glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_RGBA8, DIM, DIM, DIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  GLint width;
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &width);

  if ( (width==0) || ( DIM > GL_MAX_TEXTURE_SIZE+2 ) ){
    QMessageBox::critical(NULL,QString("Error"),"Your hardware don't support this precision");

    int n=DIM * DIM * DIM * 4;
    std::cerr<<"memory for texture: "<<n<<" byte";
    std::cerr<<" = "<<n/1024<<" kbyte";
    std::cerr<<" = "<<n/1024/1024<<" mbyte\n";
    return false;
  }
  
  WIDTH = HEIGHT = DEPTH = DIM;
  BYTES_PER_TEXEL=4;
  SIZE=WIDTH * HEIGHT * DEPTH * BYTES_PER_TEXEL;

  std::cerr<<"memory for texture: "<<SIZE<<" byte";
  std::cerr<<" = "<<SIZE/1024<<" kbyte";
  std::cerr<<" = "<<SIZE/1024/1024<<" mbyte\n";
  
  if (texelsI){
    free(texelsI);
    texelsI=NULL;
  }

  if (texels)
    free(texels);
  texels=(unsigned char *) malloc(SIZE);
  
  if (texels == NULL){
    QMessageBox::critical(NULL,QString("Error"),"Not enought memory for texture");
    std::cerr<<"Error: Not enought memory for texture\n";
    exit(0);
  }
  int s, t, r;
  unsigned char col;
  int posTex;

  double max_s=pos.R_component_1()+1/zoom;
  double min_s=pos.R_component_1()-1/zoom;
  double max_t=pos.R_component_2()+1/zoom;
  double min_t=pos.R_component_2()-1/zoom;
  double max_r=pos.R_component_3()+1/zoom;
  double min_r=pos.R_component_3()-1/zoom;
  double q=pos.R_component_4();
  
  const int iteratelimit = 104; //500;
  const double modulelimit = 4;//20;
  int aggiornamento=DEPTH/32;
  if (!aggiornamento)
    aggiornamento=1;

  for (r=1;r<DEPTH-1;r++){
    for (t=1;t<HEIGHT-1;t++){
      for (s=1;s<WIDTH-1;s++){
	double_quaternion x((double)s*((max_s-min_s)/(double)WIDTH)+min_s,
			    (double)t*((max_t-min_t)/(double)HEIGHT)+min_t,
			    (double)r*((max_r-min_r)/(double)DEPTH)+min_r,
			    q);
	double_quaternion c;
	if (usePar)
	  c=par;
	else
	  c=x;
        int i=0;
        for( i=0; (x.abs()<modulelimit) && (i<iteratelimit) ;i++) {
          x=x*x+c;
        }
	col=255-lround((double)255*((double)i/(double)iteratelimit));
	posTex=texel3(s,t,r);
	texels[ posTex + 0 ] = col;
	texels[ posTex + 1 ] = col;
	texels[ posTex + 2 ] = col;
	texels[ posTex + 3 ] = 0xff-col; 
      }
    }
    if( !(r%aggiornamento) ) //risparmio sui segnali di progress che invio per non sprecare risorse
      emit progress( (int)(100.0*r/DEPTH) );
  }
  emit progress(99);
  //setto i bordi invisibili
  for (t=0;t<HEIGHT;t++){
    for (s=0;s<WIDTH;s++){
	texels[ texel3(s,t,DEPTH-1) + 3 ] = 0; 
	texels[ texel3(s,t,0) + 3 ] = 0; 
    }
  }
  for (r=0;r<DEPTH;r++){
    for (s=0;s<WIDTH;s++){
	texels[ texel3(s,0,r) + 3 ] = 0; 
	texels[ texel3(s,HEIGHT-1,r) + 3 ] = 0; 
    }
  }
  for (r=0;r<DEPTH;r++){
    for (t=0;t<HEIGHT;t++){
      texels[ texel3(0,t,r) + 3 ] = 0; 
      texels[ texel3(WIDTH-1,t,r) + 3 ] = 0; 
    }
  }
  emit progress(100);
  rebindTexture();
  return true;
}

void staticTexture::improveTexture(int grey, const QVector<QColor> &color){
  if(texelsI)
    free(texelsI);
  texelsI=NULL;
  
  emit progress(0);
  bool appG=grey!=-1;
  bool appC=color.size()>=255;
  if(appG || appC){// se devo fare qualcosa
    texelsI = (unsigned char *)malloc(SIZE);
    if (!texelsI){
      QMessageBox::critical(NULL,QString("Error"),"Not enought memory for texture");
      std::cerr<<"Error: Not enought memory for texture\n";
      exit(0);
    }
    unsigned char *source,*dest;
    dest=texelsI;
    source=texels+3;
    
    double aa=100-(grey*9.0/10);
    double bb=255.0/(exp(255.0/aa)-1);
    int aggiornamento=SIZE/16;
    unsigned char r,g,b,a;
    r=g=b=0;//tmch
    for (int i=0;i<SIZE;i+=BYTES_PER_TEXEL){
      a=*source;
      if(appG){
	r=g=b=a=(unsigned char) (bb*(exp( (255-a) /aa)-1));
	a=255-a;
      }
      if(appC){
	QColor col(color[255-a]);
	r=col.red();
	g=col.green();
	b=col.blue();
	a=col.alpha();      
      }
      dest[0]=r;
      dest[1]=g;
      dest[2]=b;
      dest[3]=a;
      dest+=BYTES_PER_TEXEL;
      source+=BYTES_PER_TEXEL;
      if( !(i%aggiornamento) ) //risparmio sui segnali di progress che invio per non sprecare risorse
	emit progress((int)(100.0*i/SIZE));
    }
  }
  
  emit progress(99);
  if (texelsI){
    //setto i bordi invisibili
    int s,t,r;
    for (t=0;t<HEIGHT;t++){
      for (s=0;s<WIDTH;s++){
	texelsI[ texel3(s,t,DEPTH-1) + 3 ] = 0; 
	texelsI[ texel3(s,t,0) + 3 ] = 0; 
      }
    }
    for (r=0;r<DEPTH;r++){
      for (s=0;s<WIDTH;s++){
	texelsI[ texel3(s,0,r) + 3 ] = 0; 
	texelsI[ texel3(s,HEIGHT-1,r) + 3 ] = 0; 
      }
    }
    for (r=0;r<DEPTH;r++){
      for (t=0;t<HEIGHT;t++){
	texelsI[ texel3(0,t,r) + 3 ] = 0; 
	texelsI[ texel3(WIDTH-1,t,r) + 3 ] = 0; 
      }
    }
  }
  rebindTexture();
  emit progress(100);
}

void staticTexture::rebindTexture(){
  glBindTexture(GL_TEXTURE_3D, texname);
  
  //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
  
  if (texelsI){
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texelsI);
  }
  else{
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
  }
  checkGlError("staticTexture::rebindTexture()");
}

void staticTexture::bindTexture(float *s,float *t,float *r){
  glBindTexture(GL_TEXTURE_3D,texname);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  
  glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
  glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
  glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);

  glTexGenfv(GL_S,GL_OBJECT_PLANE,s);
  glTexGenfv(GL_T,GL_OBJECT_PLANE,t);
  glTexGenfv(GL_R,GL_OBJECT_PLANE,r);  
}

bool staticTexture::save(QDataStream &s){
  s<<(qint32)WIDTH<< (qint32)HEIGHT<< (qint32)DEPTH<< (qint32)BYTES_PER_TEXEL<< (qint32)SIZE;
  for (int n=0;n<SIZE;n+=BYTES_PER_TEXEL)
    s<<(qint8)texels[n];
  return true;
}

bool staticTexture::load(QDataStream &s){
  qint32 tmp;
  s>>tmp;
  WIDTH=tmp;
  s>>tmp;
  HEIGHT=tmp;  
  s>>tmp;
  DEPTH=tmp;
  s>>tmp;
  BYTES_PER_TEXEL=tmp;
  s>>tmp;
  SIZE=tmp;
  qint8 tmp2;
  if (texels)
    free(texels);
  if (texelsI){
    free(texelsI);
    texelsI=NULL;
  }

  texels = (unsigned char *)malloc(SIZE);
  for (int n=0;n<SIZE;n+=BYTES_PER_TEXEL){
    s>>tmp2;
    texels[n]=tmp2;
    texels[n+1]=tmp2;
    texels[n+2]=tmp2;
    texels[n+3]=255-tmp2;
  }
  rebindTexture();
  return true;
}


//===================================================================================
void checkGlError(const char *msg){
  QString errore("Error ");
  if (msg){
    errore+="on ";
    errore+=msg;
  }
  errore+=": ";
  GLenum err=glGetError();
  if (err!=GL_NO_ERROR){
    switch(err){
    case GL_INVALID_ENUM:
      errore+=QString("GL_INVALID_ENUM");
      break;
    case GL_INVALID_VALUE:
      errore+=QString("GL_INVALID_VALUE");
      break;
    case GL_INVALID_OPERATION:
      errore+=QString("GL_INVALID_OPERATION");
      break;
    case GL_STACK_OVERFLOW:
      errore+=QString("GL_STACK_OVERFLOW");
      break;
    case GL_STACK_UNDERFLOW:
      errore+=QString("GL_STACK_UNDERFLOW");
      break;
    case GL_OUT_OF_MEMORY:
      errore+=QString("GL_OUT_OF_MEMORY");
      break;
    default:
      errore+=QString("unknown error");
      break;
    }
    QMessageBox::critical(NULL,QString("ERROR"),errore);
    QTextStream out(stderr);
    out<<errore<<"\n";
    //exit (0);
  }
}

