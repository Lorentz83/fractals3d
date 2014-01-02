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


varying vec4 rotatedVertex;
uniform bool lightEnable;

uniform vec4 textureRotationX;
uniform vec4 textureRotationY;
uniform vec4 textureRotationZ;

vec4 Ambient;
vec4 Diffuse;
vec4 Specular;
const vec4 AMBIENT_BLACK = vec4(0.0, 0.0, 0.0, 0.0);


void Light(vec3 normal, vec3 WSP3){
   float nDotVP;       // normale direzione luce
   float nDotHV;       // normale luce
   float pf;           // Potenza luce
   float attenuation;  // fattore attenuazione luce
   float d;            // distanza piano/luce
   vec3  VP;           // direzione vettore piano/luce
   vec3  halfVector;   // direzione della luce
   vec3 eye = vec3(0.0,0.0,-1.0);
   // Calcolo il vettore piano/luce
   VP = vec3 (gl_LightSource[0].position) - WSP3;

   // Calcolo la distanza piano/luce
   d = length(VP);

   // Normalizzo il vettore piano/luce
   VP = normalize(VP);

   // Calcolo l'atenuazione
//    attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +
//        gl_LightSource[0].linearAttenuation * d +
//        gl_LightSource[0].quadraticAttenuation * d * d);

   halfVector = normalize(VP + eye);

   nDotVP = max(0.0, dot(normal, VP));
   nDotHV = max(0.0, dot(normal, halfVector));

   if (nDotVP == 0.0){
       pf = 0.0;
   }
   else{
       pf = pow(nDotHV, gl_FrontMaterial.shininess);
   }
//    Ambient  += gl_LightSource[0].ambient * attenuation;
//    Diffuse  += gl_LightSource[0].diffuse * nDotVP * attenuation;
//    Specular += gl_LightSource[0].specular * pf * attenuation;
   Ambient  += gl_LightSource[0].ambient;
   Diffuse  += gl_LightSource[0].diffuse * nDotVP ;
   Specular += gl_LightSource[0].specular * pf ;
}

bool isLightEnabled(int i){
	// Controllo la luce se e' nera allora la disattivo
	bool enabled = true;
	if ((gl_LightSource[i].ambient  == AMBIENT_BLACK) && (gl_LightSource[i].diffuse  == AMBIENT_BLACK))
		enabled = false;
	return(enabled);
}

void main (){
	vec3 normal;
	vec4 vertexObject;
	float alphaFade = 1.0;
	float dtX,dtY,dtZ;

	Ambient=vec4(0.0);
	Diffuse=vec4(0.0);
	Specular=vec4(0.0);

	vec4 color=vec4(1.0,1.0,1.0,0.0);
	vec4 wordSpacePosition=gl_ModelViewMatrix * gl_Vertex;
	vec3 WSP3 = (vec3 (wordSpacePosition)) / wordSpacePosition.w;
	vertexObject=vec4(gl_Vertex); //passo al fragment shader il vettore con le coordinate in sapzio oggetto perche' mi servono per calcolare la texture 
	dtX=(dot(textureRotationX,vertexObject)-textureRotationX.w)/length(textureRotationX.xyz);
	dtY=(dot(textureRotationY,vertexObject)-textureRotationY.w)/length(textureRotationY.xyz);
	dtZ=(dot(textureRotationZ,vertexObject)-textureRotationZ.w)/length(textureRotationZ.xyz);
	rotatedVertex=vec4(dtX,dtY,dtZ,vertexObject.w); //coordinate di rotazione della texture

	normal = normalize(gl_NormalMatrix * gl_Normal);
	if(isLightEnabled(0)){
		Light(normal,WSP3);
		color = gl_FrontLightModelProduct.sceneColor +
		Ambient  * gl_FrontMaterial.ambient +
		Diffuse  * gl_FrontMaterial.diffuse;
		color += Specular * gl_FrontMaterial.specular;
		color = clamp( color, 0.0, 1.0 );
		lightEnable=true;
	}
	else{
		lightEnable=false;
	}
	gl_FrontColor = color;
	gl_FrontColor.a *= alphaFade;
	gl_Position=ftransform();
}
