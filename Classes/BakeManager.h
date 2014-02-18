/*!
 * \file BakeManager.h
 * \brief Entrypoint for baker program
 * \author Jijidici
 * \date 18/02/2014
 */

#ifndef __BAKE_MANAGER_H__
#define __BAKE_MANAGER_H__

#include "cocos2d.h"

USING_NS_CC;

class BakeManager : public Layer {
public:
	/*! \brief Destructor */
	~BakeManager();

	static BakeManager* create();

	virtual bool init();
	virtual void update(float fDt);

protected:
	/*! \brief Constructor */
	BakeManager();
};

#endif //__BAKE_MANAGER_H__