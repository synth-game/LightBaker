/*!
 * \file LightMap.h
 * \brief Physic data of lighting
 * \author Jijidici
 * \date 19/02/2014
 */

#ifndef __LIGHT_MAP_H__
#define __LIGHT_MAP_H__

#include <vector>

class LightMap {
public:
	LightMap(int iW, int iH, int iCoef);
	~LightMap();

	void addLight(int iLightId);
	void saveToXml(const char* filePath);

protected:
	int	_iW;
	int _iH;
	int _iResolutionCoef;
	std::vector<std::vector<std::pair<int, bool>>> _pixelGrid;
};

#endif //__LIGHT_MAP_H__