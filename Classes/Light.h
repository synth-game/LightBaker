/*!
 * \file Light.h
 * \brief Stores a light data
 * \author Jijidici
 * \date 19/02/2014
 */

#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "cocos2d.h"

USING_NS_CC;

class Light {
public:
	Light(Point pos, Point dir, float fAperture);
	~Light();

	Point getPosition() { return _position; }
	Point getDirection() { return _direction; }
	float getAperture() { return _fAperture; }

protected:
	Point _position;
	Point _direction;
	float _fAperture;
};

#endif //__LIGHT_H__