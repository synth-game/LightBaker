/*!
 * \file BakeManager.cpp
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

#define _USE_MATH_DEFINES

#include <cmath>
#include <sstream>
#include "BakeManager.h"
#include "LightMap.h"
#include "SHA_light_baking.h"
#include "SHA_blur.h"

#define RES_COEF 4

BakeManager::BakeManager() 
	: Layer() 
	, _pRenderTex(nullptr)
	, _pBitmask(nullptr)
	, _pLight(nullptr)
	, _pLightBakingProgram(nullptr)
	, _pBlurProgram(nullptr)
	, _iLightCursor(0)
	, _iLevelCursor(0) {

}

BakeManager::~BakeManager() {
	for(std::vector<Light*>::iterator itLight=_lights.begin(); itLight!=_lights.end(); ++itLight) {
		delete *itLight;
	}
	_lights.clear();
	_levelNames.clear();
}

BakeManager* BakeManager::create() {
	BakeManager* pRet = new BakeManager();
	if(pRet != nullptr && pRet->init()) {
		CCLOG("BakeManager created");
		pRet->autorelease();
	} else {
		CCLOG("BakeManager created but deleted");
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

bool BakeManager::init() {
	bool bRet = Layer::init();

	// activate update function
	scheduleUpdate();

	// load level name
	tinyxml2::XMLDocument levelsDoc;
	int xmlerror = levelsDoc.LoadFile("xml/levels.xml");
	CCASSERT(xmlerror==0, "ERROR LOADING LEVELS XML FILE");
	tinyxml2::XMLElement* pGameElt = levelsDoc.FirstChildElement("game");
	tinyxml2::XMLElement* pLevelElt = pGameElt->FirstChildElement("level");
	while(pLevelElt != nullptr) {
		_levelNames.push_back(pLevelElt->Attribute("name"));

		pLevelElt = pLevelElt->NextSiblingElement("level");
	}

	// create shaders
	_pLightBakingProgram = new GLProgram();
	_pLightBakingProgram->initWithVertexShaderByteArray(spotLighting_vert, spotLighting_frag);
	_pLightBakingProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	_pLightBakingProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	_pLightBakingProgram->link();
	_pLightBakingProgram->updateUniforms();

	_pBlurProgram = new GLProgram();
	_pBlurProgram->initWithVertexShaderByteArray(blur_vert, blur_frag);
	_pBlurProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	_pBlurProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	_pBlurProgram->link();
	_pBlurProgram->updateUniforms();
	
	loadLevel(_iLevelCursor);

	return bRet;
}

void BakeManager::loadLevel(int iLevelId) {
	if(iLevelId < _levelNames.size()) {
		// load the bitmask
		_pBitmask = Sprite::create(std::string("levels/"+_levelNames[iLevelId]+"/bitmask.png").c_str());
		_pBitmask->setAnchorPoint(Point::ZERO);
		_pBitmask->setScale(1.f/RES_COEF);
		Size bitmaskSize = _pBitmask->getContentSize()/RES_COEF;
		float fLevelRatio = bitmaskSize.height/bitmaskSize.width;
		if(bitmaskSize.width < bitmaskSize.height) {
			fLevelRatio = bitmaskSize.width/bitmaskSize.height;
		}
		Layer::addChild(_pBitmask);

		_pLight = Sprite::create(std::string("levels/"+_levelNames[iLevelId]+"/bitmask.png").c_str());
		_pLight->setAnchorPoint(Point::ZERO);
		_pLight->setScale(1.f/RES_COEF);
		Layer::addChild(_pLight);

		// attach custom shader to the bitmask
		_pLightBakingProgram->use();
		_pLightBakingProgram->setUniformLocationWith2f(_pLightBakingProgram->getUniformLocationForName("SY_TexSize"), bitmaskSize.width, bitmaskSize.height);
		_pBitmask->setShaderProgram(_pLightBakingProgram);

		// blur shader
		_pBlurProgram->use();
		_pBlurProgram->setUniformLocationWith2f(_pBlurProgram->getUniformLocationForName("SY_TexSize"), bitmaskSize.width, bitmaskSize.height);
		_pLight->setShaderProgram(_pBlurProgram);

		//create the render texture
		_pRenderTex = new RenderTexture();
		_pRenderTex->initWithWidthAndHeight(bitmaskSize.width, bitmaskSize.height, _pBitmask->getTexture()->getPixelFormat());
		_pRenderTex->setAnchorPoint(Point::ZERO);

		//initialize lights
		tinyxml2::XMLDocument actorsDoc;
		int xmlerror = actorsDoc.LoadFile(std::string("levels/"+_levelNames[iLevelId]+"/actors.xml").c_str());
		CCASSERT(xmlerror==0, "ERROR LOADING ACTORS XML FILE");
		tinyxml2::XMLElement* pActorElt = actorsDoc.FirstChildElement("actor");
		while(pActorElt != nullptr) {
			std::string sActorType(pActorElt->Attribute("type"));
			if(sActorType == "LIGHT") {
				tinyxml2::XMLElement* pComponentElt = pActorElt->FirstChildElement("component");
				Point lightPos;
				Point lightDir;
				float fAperture;
				float fLength;
				while(pComponentElt != nullptr) {
					std::string sCompType(pComponentElt->Attribute("type"));
					if(sCompType == "GEOMETRY") {
						// add a new light
						tinyxml2::XMLElement* pPositionElt = pComponentElt->FirstChildElement("position");
						lightPos.x = pPositionElt->FloatAttribute("x");
						lightPos.y = _pBitmask->getContentSize().height - pPositionElt->FloatAttribute("y");
						float fRotate = atof(pComponentElt->FirstChildElement("rotate")->GetText()) - 90.f;
						lightDir = Point(-cos(fRotate*M_PI/180.f), sin(fRotate*M_PI/180.f));
					}

					pComponentElt = pComponentElt->NextSiblingElement("component");
				}

				tinyxml2::XMLElement* pLightBakingElt = pActorElt->FirstChildElement("light_baking");
				CCASSERT(pLightBakingElt != nullptr, "There is no light_baking tag");
				fAperture = atof(pLightBakingElt->FirstChildElement("aperture")->GetText())*fLevelRatio /2.f;
				fLength = atof(pLightBakingElt->FirstChildElement("length")->GetText())/RES_COEF;

				Light* pNewLight = new Light(lightPos, lightDir, fAperture, fLength);
				_lights.push_back(pNewLight);
			}

			pActorElt = pActorElt->NextSiblingElement("actor");
		}

	} else {
		//quit the program
		CCLOG("END OF PROCESS");
		Director::getInstance()->end();
	}
}

void BakeManager::clearLevel() {
	Layer::removeChild(_pBitmask);
	Layer::removeChild(_pLight);
	delete _pRenderTex;

	for(auto light : _lights) {
		delete light;
		light = nullptr;
	}
	_lights.clear();
	_iLightCursor = 0;
}

void BakeManager::update(float fDt) {

	// # FIRST PASS - light renderning
	_pRenderTex->clear(0.f, 0.f, 0.f, 0.f);
	_pRenderTex->begin();

	//draw light
	if (!_lights.empty()) {
		Light* pCurrentLight = _lights[_iLightCursor];

		_pLightBakingProgram->use();
		_pLightBakingProgram->setUniformLocationWith2f(_pLightBakingProgram->getUniformLocationForName("SY_LightPos"), pCurrentLight->getPosition().x/RES_COEF, pCurrentLight->getPosition().y/RES_COEF);
		_pLightBakingProgram->setUniformLocationWith2f(_pLightBakingProgram->getUniformLocationForName("SY_LightDir"), pCurrentLight->getDirection().x, pCurrentLight->getDirection().y);
		_pLightBakingProgram->setUniformLocationWith1f(_pLightBakingProgram->getUniformLocationForName("SY_Aperture"), pCurrentLight->getAperture());
		_pLightBakingProgram->setUniformLocationWith1f(_pLightBakingProgram->getUniformLocationForName("SY_Length"), pCurrentLight->getLength());
		_pBitmask->visit();

		_pRenderTex->end();

		// # SECOND PASS - blur
		Texture2D* pLightTex = new Texture2D();
		pLightTex->autorelease();
		pLightTex->initWithImage(_pRenderTex->newImage());
		_pLight->setTexture(pLightTex);
		_pRenderTex->clear(0.f, 0.f, 0.f, 0.f);
		_pRenderTex->begin();

		_pLight->visit();

		_pRenderTex->end();

		// save the light texture in PNG
		std::stringstream filePath;
		filePath << "levels/" << _levelNames[_iLevelCursor] << "/PREC_light_" << _iLightCursor <<".png";
		_pRenderTex->newImage()->saveToFile(filePath.str().c_str(), false);
		++_iLightCursor;
	}
	// finish current level
	if(_lights.empty() || static_cast<unsigned int>(_iLightCursor) >= _lights.size()) {
		buildAndSaveLightmap();

		++_iLevelCursor;
		clearLevel();
		loadLevel(_iLevelCursor);
	}
}

void BakeManager::buildAndSaveLightmap() {
	LightMap* pLMap = new LightMap(_pBitmask->getContentSize().width, _pBitmask->getContentSize().height, 4*RES_COEF/RES_COEF, 4*RES_COEF);

	// build
	for(unsigned int i=0; i<_lights.size(); ++i) {
		std::stringstream filePath;
		filePath << "levels/" << _levelNames[_iLevelCursor] << "/PREC_light_" << i <<".png";
		pLMap->addLight(static_cast<int>(i), filePath.str());
	}

	// save
	pLMap->saveToXml(std::string("levels/"+_levelNames[_iLevelCursor]+"/PREC_lightmap.xml").c_str());

	delete pLMap;
}
