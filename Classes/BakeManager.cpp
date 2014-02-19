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

BakeManager::BakeManager() 
	: Layer() 
	, _pRenderTex(nullptr)
	, _pBitmask(nullptr) 
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
	Size bitmaskSize = _pBitmask->getContentSize();
	Layer::addChild(_pBitmask);

	// attach custom shader to the bitmask
	GLProgram* pProgram = new GLProgram();
	pProgram->initWithVertexShaderByteArray(spotLighting_vert, spotLighting_frag);
	pProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	pProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	pProgram->link();
	pProgram->updateUniforms();
	pProgram->use();
	pProgram->setUniformLocationWith2f(pProgram->getUniformLocationForName("SY_TexSize"), bitmaskSize.width, bitmaskSize.height);
	_pBitmask->setShaderProgram(pProgram);


	//create the render texture
	_pRenderTex = new RenderTexture();
	_pRenderTex->initWithWidthAndHeight(bitmaskSize.width, bitmaskSize.height, _pBitmask->getTexture()->getPixelFormat());

	// initialize lights
	_lights.push_back(new Light(Point(490.f, 260.f), Point(0.f, -1.f), 30.));
	_lights.push_back(new Light(Point(540.f, 260.f), Point(0.f, -1.f), 30.));
	_lights.push_back(new Light(Point(590.f, 260.f), Point(0.f, -1.f), 30.));

	return bRet;
}

void BakeManager::update(float fDt) {

	// lights rendering
	_pRenderTex->clear(0.f, 0.f, 0.f, 0.f);
	_pRenderTex->begin();

	//draw light
	GLProgram* pBMProgram = _pBitmask->getShaderProgram();
	Light* pCurrentLight = _lights[_iLightCursor];

	pBMProgram->use();
	pBMProgram->setUniformLocationWith2f(pBMProgram->getUniformLocationForName("SY_LightPos"), pCurrentLight->getPosition().x, pCurrentLight->getPosition().y);
	pBMProgram->setUniformLocationWith2f(pBMProgram->getUniformLocationForName("SY_LightDir"), pCurrentLight->getDirection().x, pCurrentLight->getDirection().y);
	pBMProgram->setUniformLocationWith1f(pBMProgram->getUniformLocationForName("SY_Aperture"), pCurrentLight->getAperture());
	_pBitmask->draw();

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
	LightMap* pLMap = new LightMap(_pBitmask->getContentSize().width, _pBitmask->getContentSize().height);

	// build
	for(unsigned int i=0; i<_lights.size(); ++i) {
		pLMap->addLight(static_cast<int>(i));
	}

	// save

	delete pLMap;
}
