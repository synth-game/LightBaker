/*!
 * \file BakeManager.h
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

#ifndef __BAKE_MANAGER_H__
#define __BAKE_MANAGER_H__

#include <vector>
#include "cocos2d.h"
#include "Light.h"

USING_NS_CC;

class BakeManager : public Layer {
public:
	/*! \brief Destructor */
	~BakeManager();

	static BakeManager* create();

	virtual bool init();
	virtual void update(float fDt);
	void buildAndSaveLightmap();

protected:
	/*! \brief Constructor */
	BakeManager();

	RenderTexture* _pRenderTex;
	Sprite* _pBitmask;
	int	_iLightCursor;
	std::vector<Light*> _lights;
};

#endif //__BAKE_MANAGER_H__