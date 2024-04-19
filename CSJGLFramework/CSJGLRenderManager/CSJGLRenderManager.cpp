#include "CSJGLRenderManager.h"

#include "CSJGLRenderManagerImpl.h"

CSJSharedRenderManger CSJGLRenderManager::getDefaultRenderManager() {
    return CSJGLRenderManagerImpl::getInstance();
}