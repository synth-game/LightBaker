/*!
 * \file LightMap.cpp
 * \brief Physic data of lighting
 * \author Jijidici
 * \date 19/02/2014
 */

#include <sstream>
#include "cocos2d.h"
#include "LightMap.h"

USING_NS_CC;

LightMap::LightMap(int iW, int iH)
	: _iW(iW)
	, _iH(iH) {
		_pixelGrid.resize(_iW*_iH);
}

LightMap::~LightMap() {
	for(std::vector<std::vector<std::pair<int, bool>>>::iterator itPixel=_pixelGrid.begin(); itPixel!=_pixelGrid.end(); ++itPixel) {
		itPixel->clear();
	}
	_pixelGrid.clear();
}

void LightMap::addLight(int iLightId) {
	std::stringstream texPath;
	texPath<<"levels/test/PREC_light_"<<iLightId<<".png";

	Image* pLightTexture = new Image();
	pLightTexture->initWithImageFile(texPath.str().c_str());

	for(int j=0; j<_iH; ++j) {
		for(int i=0; i<_iW; ++i) {
			int index = i + j*_iW;
			if(pLightTexture->getData()[4*index] == 255) {
				bool bOcculted = false;
				if(pLightTexture->getData()[4*index+1] == 255) {
					bOcculted = true;
				}

				_pixelGrid[index].push_back(std::make_pair(iLightId, bOcculted));
			}
		}
	}

	delete pLightTexture;
}

void LightMap::saveToXml(const char* filePath) {
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLElement* pRootElement = doc.NewElement("root");
	doc.InsertEndChild(pRootElement);

	// save all data
	for(std::vector<std::vector<std::pair<int, bool>>>::iterator itPixel=_pixelGrid.begin(); itPixel!=_pixelGrid.end(); ++itPixel) {
		tinyxml2::XMLElement* pPixelElement = doc.NewElement("pixel");
		pRootElement->InsertEndChild(pPixelElement);

		for(std::vector<std::pair<int, bool>>::iterator itLight=itPixel->begin(); itLight!=itPixel->end(); ++itLight) {
			tinyxml2::XMLElement* pLightSampleElement = doc.NewElement("light_sample");
			pLightSampleElement->SetAttribute("id", itLight->first);
			pLightSampleElement->SetAttribute("occulted", itLight->second);
			pPixelElement->InsertEndChild(pLightSampleElement);
		}
	}

	doc.SaveFile(filePath);
}