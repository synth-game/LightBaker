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

	vec3 computeSpotLight(vec2 position, vec2 lightPos, vec2 lightDir, float phi, float fRayStep) {
		vec2 l = normalize(lightPos - position);
		float cos_theta = dot(l, lightDir);
		float cos_phi = cos(radians(phi));
		
		vec3 retCol = vec3(0.);
		if(cos_theta > cos_phi) {
			retCol.r = 1.;

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
		
			if(isOcculted == true) {
				retCol.g = 1.;
			}
		}
		return retCol;
	}

	void main() {
		vec2 pixelSize = vec2(1./SY_TexSize.x, 1./SY_TexSize.y);
		float fRayStep = (pixelSize.x + pixelSize.y) /2.;

		vec2 realLightPos = vec2(pixelSize.x*SY_LightPos.x, pixelSize.y*SY_LightPos.y);

		vec3 color = computeSpotLight(v_texCoord, realLightPos, SY_LightDir, SY_Aperture, fRayStep);

		gl_FragColor = vec4(color, 1.);
	}

	//end
);

#endif //__SHA_LIGHT_BAKING_H__