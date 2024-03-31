#pragma once

#include "base/memory/singleton.h"

class RenderManager {
public:
	SINGLETON_DEFINE(RenderManager);

	RenderManager();
	~RenderManager();
	
	void initRenderEnv();

    void stop();

protected:
	void renderEnvCreated();
};


