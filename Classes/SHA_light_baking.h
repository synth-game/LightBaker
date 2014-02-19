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

	void main()
	{
		vec3 color = vec3(0.);

		float isWall = texture2D(CC_Texture0, v_texCoord).r;
		if(isWall > 0.9) {
			color = vec3(0., 1., 0.);
		}

		gl_FragColor = vec4(color, 1.);
	}

	//end
);

#endif //__SHA_LIGHT_BAKING_H__