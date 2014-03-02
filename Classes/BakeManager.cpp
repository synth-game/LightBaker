/*!
 * \file BakeManager.cpp
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

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
	, _iLightCursor(0) {

}

BakeManager::~BakeManager() {
	for(std::vector<Light*>::iterator itLight=_lights.begin(); itLight!=_lights.end(); ++itLight) {
		delete *itLight;
	}
	_lights.clear();
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

	// load the bitmask
	_pBitmask = Sprite::create("levels/test/bitmask.png");
	_pBitmask->setAnchorPoint(Point::ZERO);
	_pBitmask->setScale(1.f/RES_COEF);
	Size bitmaskSize = _pBitmask->getContentSize()/RES_COEF;
	Layer::addChild(_pBitmask);

	_pLight = Sprite::create("levels/test/bitmask.png");
	_pLight->setAnchorPoint(Point::ZERO);
	_pLight->setScale(1.f/RES_COEF);
	Layer::addChild(_pLight);

	// attach custom shader to the bitmask
	_pLightBakingProgram = new GLProgram();
	_pLightBakingProgram->initWithVertexShaderByteArray(spotLighting_vert, spotLighting_frag);
	_pLightBakingProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	_pLightBakingProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	_pLightBakingProgram->link();
	_pLightBakingProgram->updateUniforms();
	_pLightBakingProgram->use();
	_pLightBakingProgram->setUniformLocationWith2f(_pLightBakingProgram->getUniformLocationForName("SY_TexSize"), bitmaskSize.width, bitmaskSize.height);
	_pBitmask->setShaderProgram(_pLightBakingProgram);

	// blur shader
	_pBlurProgram = new GLProgram();
	_pBlurProgram->initWithVertexShaderByteArray(blur_vert, blur_frag);
	_pBlurProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	_pBlurProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	_pBlurProgram->link();
	_pBlurProgram->updateUniforms();
	_pBlurProgram->use();
	_pBlurProgram->setUniformLocationWith2f(_pBlurProgram->getUniformLocationForName("SY_TexSize"), bitmaskSize.width, bitmaskSize.height);
	_pLight->setShaderProgram(_pBlurProgram);


	//create the render texture
	_pRenderTex = new RenderTexture();
	_pRenderTex->initWithWidthAndHeight(bitmaskSize.width, bitmaskSize.height, _pBitmask->getTexture()->getPixelFormat());
	_pRenderTex->setAnchorPoint(Point::ZERO);

	// initialize lights
	_lights.push_back(new Light(Point(390.f, 460.f), Point(0.f, -1.f), 30.));
	_lights.push_back(new Light(Point(590.f, 460.f), Point(0.f, -1.f), 30.));
	_lights.push_back(new Light(Point(790.f, 460.f), Point(0.f, -1.f), 30.));

	return bRet;
}

void BakeManager::update(float fDt) {

	// # FIRST PASS - light renderning
	_pRenderTex->clear(0.f, 0.f, 0.f, 0.f);
	_pRenderTex->begin();

	//draw light
	Light* pCurrentLight = _lights[_iLightCursor];

	_pLightBakingProgram->use();
	_pLightBakingProgram->setUniformLocationWith2f(_pLightBakingProgram->getUniformLocationForName("SY_LightPos"), pCurrentLight->getPosition().x/RES_COEF, pCurrentLight->getPosition().y/RES_COEF);
	_pLightBakingProgram->setUniformLocationWith2f(_pLightBakingProgram->getUniformLocationForName("SY_LightDir"), pCurrentLight->getDirection().x, pCurrentLight->getDirection().y);
	_pLightBakingProgram->setUniformLocationWith1f(_pLightBakingProgram->getUniformLocationForName("SY_Aperture"), pCurrentLight->getAperture());
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
	filePath << "levels/test/PREC_light_" << _iLightCursor <<".png";
	_pRenderTex->newImage()->saveToFile(filePath.str().c_str(), false);
	++_iLightCursor;

	// exit the program
	if(static_cast<unsigned int>(_iLightCursor) >= _lights.size()) {
		buildAndSaveLightmap();
		Director::getInstance()->end();
	}
}

void BakeManager::buildAndSaveLightmap() {
	LightMap* pLMap = new LightMap(_pBitmask->getContentSize().width/RES_COEF, _pBitmask->getContentSize().height/RES_COEF, 1);

	// build
	for(unsigned int i=0; i<_lights.size(); ++i) {
		pLMap->addLight(static_cast<int>(i));
	}

	// save
	pLMap->saveToXml("levels/test/PREC_lightmap.xml");

	delete pLMap;
}
