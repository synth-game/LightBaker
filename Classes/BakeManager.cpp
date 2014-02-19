/*!
 * \file BakeManager.cpp
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

#include "BakeManager.h"
#include "SHA_light_baking.h"

BakeManager::BakeManager() 
	: Layer() 
	, _pRenderTex(nullptr)
	, _pBitmask(nullptr) {

}

BakeManager::~BakeManager() {

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
	Layer::addChild(_pBitmask);

	// attach custom shader to the bitmask
	GLProgram* pProgram = new GLProgram();
	pProgram->initWithVertexShaderByteArray(spotLighting_vert, spotLighting_frag);
	pProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	pProgram->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	pProgram->link();
	pProgram->updateUniforms();
	_pBitmask->setShaderProgram(pProgram);


	//create the render texture
	_pRenderTex = new RenderTexture();
	_pRenderTex->initWithWidthAndHeight(_pBitmask->getContentSize().width, _pBitmask->getContentSize().height, _pBitmask->getTexture()->getPixelFormat());

	return bRet;
}

void BakeManager::update(float fDt) {

	// lights rendering
	_pRenderTex->clear(0.f, 0.f, 0.f, 0.f);
	_pRenderTex->begin();

	//draw light
	_pBitmask->draw();

	_pRenderTex->end();

	// save the light texture in PNG
	_pRenderTex->saveToFile("levels/test/PREC_light_01.png");

	// exit the program
	Director::getInstance()->end();
}
