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
		
		vec3 color = texture2D(CC_Texture0, v_texCoord).bgr;

		gl_FragColor = vec4(color, 1.);
	}

	//end
);

#endif //__SHA_BLUR_H__