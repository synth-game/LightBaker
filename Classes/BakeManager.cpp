/*!
 * \file BakeManager.cpp
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

#include "BakeManager.h"

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

	//create the render texture
	_pRenderTex = new RenderTexture();
	_pRenderTex->initWithWidthAndHeight(_pBitmask->getContentSize().width, _pBitmask->getContentSize().height, _pBitmask->getTexture()->getPixelFormat());

	return bRet;
}

void BakeManager::update(float fDt) {

}
