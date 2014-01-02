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


#ifndef __GEOMETRY_H
#define __GEOMETRY_H


#include <cmath>

template<typename T>
class point3{
  T coord[3];
public:
  point3(T x,T y, T z){
    coord[0]=x;
    coord[1]=y;
    coord[2]=z;
  }
  point3(const point3 &a){
    coord[0]=a.coord[0];
    coord[1]=a.coord[1];
    coord[2]=a.coord[2];    
  }
  point3(){
    coord[0]=coord[1]=coord[2]=0;
  }
  point3<T> operator+(const point3<T> &a) const{
    return point3<T>(coord[0]+a.coord[0],coord[1]+a.coord[1],coord[2]+a.coord[2]);
  }
  point3<T> operator-(const point3<T> &a) const{
    return point3<T>(coord[0]-a.coord[0],coord[1]-a.coord[1],coord[2]-a.coord[2]);
  }
  point3<T> operator-() const{
    return point3<T>(-coord[0], -coord[1], -coord[2]);
  }
  point3<T> operator%(const point3<T> &b) const{ //cross product
    return point3<T>(coord[1]*b.coord[2]-coord[2]*b.coord[1],
		     -(coord[0]*b.coord[2]-coord[2]*b.coord[0]),
		     coord[0]*b.coord[1]-coord[1]*b.coord[0]);
  }
  T operator*(const point3 &p) const{ //dot product
    return coord[0]*p.coord[0] + coord[1]*p.coord[1] + coord[2]*p.coord[2];
  }
  point3<T> operator/(T d) const{
    return point3(coord[0]/d,coord[1]/d,coord[2]/d);
  }
  point3<T> operator*(T d) const{
    return point3(coord[0]*d,coord[1]*d,coord[2]*d);
  }
  point3<T> operator=(const point3<T> p){
    coord[0]=p.coord[0];
    coord[1]=p.coord[1];
    coord[2]=p.coord[2];
    return *this;
  }
  T module() const{
    return sqrt(coord[0]*coord[0]+coord[1]*coord[1]+coord[2]*coord[2]);
  }
  void normalize(){
    T m=module();
    if (m!=0){
      coord[0]/=m;
      coord[1]/=m;
      coord[2]/=m;
    }
  }
  point3 toNormal() const{
    point3<T> val(*this);
    val.normalize();
    return val;
  }
  T x()const{
    return coord[0];
  }
  T y()const{
    return coord[1];
  }
  T z()const{
    return coord[2];
  }
  void setX(T t){ 
    coord[0]=t;
  }
  void setY(T t){
    coord[1]=t;
  }
  void setZ(T t){
    coord[2]=t;
  }
};

typedef point3<double> double_point3;

template<typename T>
class quaternion{
  T w,x,y,z;
public:
  quaternion(){
    x=y=z=w=0;
  }
  quaternion(T ww, T xx, T yy, T zz){
    w=ww;
    x=xx;
    y=yy;
    z=zz;
  }
  quaternion<T> getConjugate() const{
    return quaternion<T>(w, -x, -y, -z);
  }
  quaternion<T> operator* (const quaternion<T> &q) const{
    return quaternion<T>(w * q.w - x * q.x - y * q.y - z * q.z ,
			 w * q.x + q.w * x + y * q.z - z * q.y ,
			 w * q.y + y * q.w + z * q.x - x * q.z ,
			 w * q.z + z * q.w + x * q.y - y * q.x);
  }
  quaternion<T> operator+ (const quaternion<T> &q) const{
    return quaternion<T> ( w+q.w , x+q.x , y+q.y , z+q.z );
  }
  bool operator==(const quaternion<T> &q) const{
    return ( w==q.w && x==q.x && y==q.y && z==q.z );
  }

  bool operator!=(const quaternion<T> &q) const{
    return ! (*this==q);
  }

  T abs(){
    return sqrt (w*w + x*x + y*y + z*z);
  }
  void normalize(){
    T mod = sqrt(w*w + x*x + y*y + z*z);
    w/=mod;
    x/=mod;
    y/=mod;
    z/=mod;
  }
  point3<T> operator* (const point3<T> &p) const{
    quaternion<T> vecQuat(0,p.x(),p.y(),p.z()), resQuat;
    resQuat = (*this) * vecQuat * getConjugate();
    return double_point3(resQuat.x,resQuat.y,resQuat.z);
  }
  T R_component_1() const{
    return w;
  }
  T R_component_2() const{
    return x;
  }
  T R_component_3() const{
    return y;
  }
  T R_component_4() const{
    return z;
  }
};


  typedef quaternion<double> double_quaternion;


#endif
