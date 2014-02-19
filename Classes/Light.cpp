/*!
 * \file Light.cpp
 * \brief Stores a light data
 * \author Jijidici
 * \date 19/02/2014
 */

#include "Light.h"

Light::Light(Point pos, Point dir, float fAperture) 
	: _position(pos)
	, _direction(dir)
	, _fAperture(fAperture) {
}

Light::~Light() {

}