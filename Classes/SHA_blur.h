/*!
 * \file SHA_blur.h
 * \brief Shaders to blur textures
 * \author Jijidici
 * \date 18/02/2014
 */

#include "GL/glew.h"

#ifndef __SHA_BLUR_H__
#define __SHA_BLUR_H__

#ifndef GL_STRINGIFY
#define GL_STRINGIFY(s) #s
#endif

const GLchar* blur_vert =
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

const GLchar* blur_frag =
GL_STRINGIFY(
	//begin

	varying vec2 v_texCoord;

	uniform sampler2D CC_Texture0;
	uniform vec2 SY_TexSize;

	void main() {
		vec2 pixelSize = vec2(1./SY_TexSize.x, 1./SY_TexSize.y);
		
		int range = 2;
		vec3 color = vec3(0.f);
		for(float fX = v_texCoord.x-(range*pixelSize.x); fX <v_texCoord.x+((range+1)*pixelSize.x); fX+=pixelSize.x) {
			for(float fY = v_texCoord.y-(range*pixelSize.y); fY <v_texCoord.y+((range+1)*pixelSize.y); fY+=pixelSize.y) {
				color += texture2D(CC_Texture0, vec2(fX, fY)).rgb;
			}
		}
		color /= 9.;

		gl_FragColor = vec4(color, 1.);
	}

	//end
);

#endif //__SHA_BLUR_H__