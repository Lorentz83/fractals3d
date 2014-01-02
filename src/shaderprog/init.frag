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


varying vec4 rotatedVertex; //vettore con le coordinate spazio oggetto ruotate con la texture
uniform bool lightEnable;

void main(){
 	vec4 color;
	vec3 cf, ct;  //colore finale con e senza luce
	float af, at; //alpha con e senza luce
	color.r = rotatedVertex.z;
	color.g = rotatedVertex.x;
	color.b = rotatedVertex.y;
	color.a = 1.0;
	
	color=(color+vec4(1.0,1.0,1.0,1.0))/2.0;
	
	if(lightEnable){
		ct = color.rgb;
 		at = color.a;
		cf = gl_Color.rgb;
		af = gl_Color.a;
 		color=vec4(ct*cf,at*af);
	}
	gl_FragColor=color;
 }
