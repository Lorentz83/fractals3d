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


varying vec4 rotatedVertex; //vettore con le coordinate spazio oggetto
uniform bool lightEnable;

/*
// tolti perche' cabalati a codice in coseguenza all'eliminazione del
// for per compatibilita' con le vecchie schede

uniform vec4 quat_c; //quaternione di somma
uniform float iteralimit; //limite iterazioni
uniform float modulelimit; //limite controllo for
uniform vec4  param[2]; //array di vettori usato per passare il quaternione da openGL
uniform float flag; //flag di scelta tra julia e mandelbrot

//compilo il quaternione con i paramtri dati in ingresso
vec4 compile_quaternion(float max_s,float min_s,float max_r, float min_r,float max_t, float min_t, float max_time, float min_time){
	vec4 quaternione;
	quaternione[0]=(vertexObject.x*(max_s-min_s))/2.0;
	quaternione[1]=(vertexObject.y*(max_t-min_t))/2.0;
	quaternione[2]=(vertexObject.z*(max_r-min_r))/2.0;
	quaternione[3]=(vertexObject.w*(max_time-min_time))/2.0;
	return quaternione;
}
*/

//modulo del quaternione
float absolute (vec4 quaternione){
	return dot(quaternione,quaternione);
}

//moltiplicazione tra que quaternioni
vec4 multquat(vec4 quat1, vec4 quat2){
	vec4 quaternione;
	quaternione.x=quat1.x*quat2.x-dot(quat1.yzw,quat2.yzw);
	quaternione.yzw=quat1.x*quat2.yzw+quat2.x*quat1.yzw+cross(quat1.yzw,quat2.yzw);
	return quaternione;
}


void main(){
	vec3 cf, ct;  //colore finale con e senza luce
	float af, at; //alpha con e senza luce
	vec4 color,quaternion,c; //vettori del colore, del quaternione e del quaternione da sommare
/*

// codice di shader originale (prima della sostituzione con gli if)
// tenuto perche' almeno sappiamo cosa fa senza leggerci miriadi di if.
// per riutilizzarlo vanno aggiunte alcune variabili

	quaternion=compile_quaternion(param[0][0],param[0][1],param[0][2],param[0][3],param[1][0],param[1][1],param[1][2],param[1][3]);
	if (flag==0.0)
 		c=quaternion;
	else
		c=quat_c;
	for (i=0.0;(i<iteralimit);i++){
		quaternion=multquat(quaternion,quaternion);
		quaternion=quaternion+c;
		col=(absolute(quaternion)<modulelimit)? col+1.0/iteralimit:col;
	}
*/	

	color=vec4(1.0,1.0,1.0,0.0);

	/*%init%*/
	
	/*%par%*/

	/*%for%*/

	if(lightEnable){
		ct = color.rgb;
 		at = color.a;
		cf = gl_Color.rgb;
		af = gl_Color.a;
 		color=vec4(ct*cf,at*af);
	}
	gl_FragColor = color;
 }
