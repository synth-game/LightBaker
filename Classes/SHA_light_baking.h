/*!
 * \file SHA_light_baking.h
 * \brief Shaders for computing light cone and occlusion
 * \author Jijidici
 * \date 18/02/2014
 */

#include "GL/glew.h"

#ifndef __SHA_LIGHT_BAKING_H__
#define __SHA_LIGHT_BAKING_H__

#ifndef GL_STRINGIFY
#define GL_STRINGIFY(s) #s
#endif

const GLchar* spotLighting_vert =
GL_STRINGIFY(
	//begin

	attribute vec4 a_position;
	attribute vec2 a_texCoord;

	varying vec2 v_texCoord;

	void main()
	{
		gl_Position = CC_MVPMatrix * a_position;
		v_texCoord = a_texCoord;
	}

	//end
);

const GLchar* spotLighting_frag =
GL_STRINGIFY(
	//begin

	varying vec2 v_texCoord;

	uniform sampler2D CC_Texture0;
	uniform vec2 SY_TexSize;
	uniform vec2 SY_LightPos;
	uniform vec2 SY_LightDir;
	uniform float SY_Aperture;
	uniform float SY_Length;

	vec4 mod289(vec4 x) {
		return x - floor(x * (1.0 / 289.0)) * 289.0;
	}

	vec4 permute(vec4 x) {
		return mod289(((x*34.0)+1.0)*x);
	}

	vec4 taylorInvSqrt(vec4 r) {
		return 1.79284291400159 - 0.85373472095314 * r;
	}

	vec2 fade(vec2 t) {
		return t*t*t*(t*(t*6.0-15.0)+10.0);
	}

	// Classic Perlin noise
	float cnoise(vec2 P) {
		vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
		vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
		Pi = mod289(Pi); // To avoid truncation effects in permutation
		vec4 ix = Pi.xzxz;
		vec4 iy = Pi.yyww;
		vec4 fx = Pf.xzxz;
		vec4 fy = Pf.yyww;

		vec4 i = permute(permute(ix) + iy);

		vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
		vec4 gy = abs(gx) - 0.5 ;
		vec4 tx = floor(gx + 0.5);
		gx = gx - tx;

		vec2 g00 = vec2(gx.x,gy.x);
		vec2 g10 = vec2(gx.y,gy.y);
		vec2 g01 = vec2(gx.z,gy.z);
		vec2 g11 = vec2(gx.w,gy.w);

		vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
		g00 *= norm.x;  
		g01 *= norm.y;  
		g10 *= norm.z;  
		g11 *= norm.w;  

		float n00 = dot(g00, vec2(fx.x, fy.x));
		float n10 = dot(g10, vec2(fx.y, fy.y));
		float n01 = dot(g01, vec2(fx.z, fy.z));
		float n11 = dot(g11, vec2(fx.w, fy.w));

		vec2 fade_xy = fade(Pf.xy);
		vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
		float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
		return 2.3 * n_xy;
	}

	vec3 computeSpotLight(vec2 position, vec2 lightPos, vec2 lightDir, float phi, float fRayStep) {
		vec2 l = normalize(lightPos - position);
		float PI = 3.14159265359;
		float doublePI = 6.28318530718;
		float cos_theta = dot(l, lightDir);
		float cos_phi = cos(radians(phi));
		
		vec3 retCol = vec3(0.);
		if(cos_theta > cos_phi && distance(lightPos, position) < SY_Length * fRayStep)  {
			retCol.r = 1.;
			retCol.b = 0.2*cnoise(vec2(100*cos_theta - (SY_LightPos.x+SY_LightPos.y)/10.));

			// occlusion computing
			vec2 samplePos = position;
			bool isOcculted = false;
			float fDotTest = 1.;
			while(fDotTest > 0.) {
				samplePos = samplePos + vec2(l.x*fRayStep, l.y*fRayStep);
				fDotTest = dot(lightPos-samplePos, lightPos-position);
			
				float fWall = texture2D(CC_Texture0, samplePos).r;
				if(fWall < 0.1 && fDotTest > 0.) {
					isOcculted = true;
					break;
				}
			}
		
			if(isOcculted == false) {
				retCol.g = 1.;
			}
		}
		return retCol;
	}

	void main() {
		vec2 pixelSize = vec2(1./SY_TexSize.x, 1./SY_TexSize.y);
		float fRayStep = (pixelSize.x + pixelSize.y) /2.;

		vec2 realLightPos = vec2(pixelSize.x*SY_LightPos.x, pixelSize.y*SY_LightPos.y);
		vec2 realLightDir = normalize(vec2(pixelSize.x*SY_LightDir.x, pixelSize.y*SY_LightDir.y));

		vec3 color = computeSpotLight(v_texCoord, realLightPos, realLightDir, SY_Aperture, fRayStep);

		gl_FragColor = vec4(color, 1.);
	}

	//end
);

#endif //__SHA_LIGHT_BAKING_H__