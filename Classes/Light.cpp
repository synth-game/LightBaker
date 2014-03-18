/*!
 * \file Light.cpp
 * \brief Stores a light data
 * \author Jijidici
 * \date 19/02/2014
 */

#include "Light.h"

Light::Light(int id, Point pos, Point dir, float fAperture, float fLength) 
	: _id(id)
	, _position(pos)
	, _direction(dir)
	, _fAperture(fAperture)
	, _fLength(fLength) {
}

Light::~Light() {

}