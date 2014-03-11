/*!
 * \file LightMap.cpp
 * \brief Physic data of lighting
 * \author Jijidici
 * \date 19/02/2014
 */

#include "cocos2d.h"
#include "LightMap.h"

USING_NS_CC;

LightMap::LightMap(int iW, int iH, int iTexCoef, int iCoef)
	: _iW(iW/iCoef)
	, _iH(iH/iCoef)
	, _iTextureResCoef(iTexCoef)
	, _iResolutionCoef(iCoef) {
		_pixelGrid.resize(_iW*_iH);
}

LightMap::~LightMap() {
	_pixelGrid.clear();
}

void LightMap::addLight(int iLightId, std::string sFilePath) {
	Image* pLightTexture = new Image();
	pLightTexture->initWithImageFile(sFilePath.c_str());

	for(int j=0; j<_iH; ++j) {
		for(int i=0; i<_iW; ++i) {
			int index = i*_iTextureResCoef + j*_iTextureResCoef*_iW*_iTextureResCoef;
			if(pLightTexture->getData()[4*index] == 255) {
				bool bOcculted = true;
				if(pLightTexture->getData()[4*index+1] == 255) {
					bOcculted = false;
				}

				_pixelGrid[i + j*_iW].push_back(std::make_pair(iLightId, bOcculted));
			}
		}
	}

	delete pLightTexture;
}

void LightMap::saveToXml(const char* filePath) {
	tinyxml2::XMLDocument* pDoc = new tinyxml2::XMLDocument();

	tinyxml2::XMLElement* pRootElement = pDoc->NewElement("root");
	pRootElement->SetAttribute("width", _iW);
	pRootElement->SetAttribute("height", _iH);
	pRootElement->SetAttribute("resolution_coef", _iResolutionCoef);
	pDoc->InsertEndChild(pRootElement);

	// save all data
	for(unsigned int i=0; i<_pixelGrid.size(); ++i) {
		if(_pixelGrid[i].size() > 0) {
			tinyxml2::XMLElement* pPixelElement = pDoc->NewElement("pixel");
			pPixelElement->SetAttribute("array_index", i);
			pRootElement->InsertEndChild(pPixelElement);

			for(std::vector<std::pair<int, bool>>::iterator itLight=_pixelGrid[i].begin(); itLight!=_pixelGrid[i].end(); ++itLight) {
				tinyxml2::XMLElement* pLightSampleElement = pDoc->NewElement("light_sample");
				pLightSampleElement->SetAttribute("id", itLight->first);
				pLightSampleElement->SetAttribute("occulted", itLight->second);
				pPixelElement->InsertEndChild(pLightSampleElement);
			}
		}
	}
	
	pDoc->SaveFile(filePath);
	CCLOG("LighMap save finished");

	delete pDoc;
}