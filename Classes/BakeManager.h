/*!
 * \file BakeManager.h
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

#ifndef __BAKE_MANAGER_H__
#define __BAKE_MANAGER_H__

#include <vector>
#include <string>
#include "cocos2d.h"
#include "Light.h"

USING_NS_CC;

class BakeManager : public Layer {
public:
	/*! \brief Destructor */
	~BakeManager();

	static BakeManager* create();

	virtual bool init();
	void loadLevel(int iLevelId);
	virtual void update(float fDt);
	void buildAndSaveLightmap();

protected:
	/*! \brief Constructor */
	BakeManager();

	RenderTexture* _pRenderTex;
	Sprite* _pBitmask;
	Sprite* _pLight;
	GLProgram* _pLightBakingProgram;
	GLProgram* _pBlurProgram;

	int	_iLightCursor;
	std::vector<Light*> _lights;

	int _iLevelCursor;
	std::vector<std::string> _levelNames;
};

#endif //__BAKE_MANAGER_H__