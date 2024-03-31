#include "RenderManager.h"

#include <windows.h>
#include <mutex>

#include "CSJDataManager.h"
#include "GLRenderImpl.h"

RenderManager::RenderManager() {
	GLRenderImpl::GetInstance()->setEnvCreatedFunc(std::bind(&RenderManager::renderEnvCreated, this));
}

RenderManager::~RenderManager() {

}

void RenderManager::initRenderEnv() {
	GLRenderImpl::GetInstance()->Init();
}

void RenderManager::renderEnvCreated() {
	
}

void RenderManager::stop() {
    GLRenderImpl::GetInstance()->StopRender();
}
