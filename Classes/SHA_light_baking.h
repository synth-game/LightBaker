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

	void main()
	{
		gl_Position = CC_MVPMatrix * a_position;
	}

	//end
);

const GLchar* spotLighting_frag =
GL_STRINGIFY(
	//begin

	void main()
	{
		gl_FragColor = vec4(0., 1., 0., 1.);
	}

	//end
);

#endif //__SHA_LIGHT_BAKING_H__