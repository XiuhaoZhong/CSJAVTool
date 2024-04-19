#ifndef __CSJGLRENDERMANAGERIMPL_H__
#define __CSJGLRENDERMANAGERIMPL_H__

#include "CSJGLRenderManager.h"

#include <vector>
#include <thread>

#include "CSJGLContext.h"

using RendererNodesArray = std::vector<CSJGLRendererNodeBase *>;

class CSJGLRenderManagerImpl : public CSJGLRenderManager {
public:
    CSJGLRenderManagerImpl();
    ~CSJGLRenderManagerImpl();

    static std::shared_ptr<CSJGLRenderManagerImpl> getInstance();

    bool initGL(HWND hwnd, int width, int height) override;
    void unInitGL() override;

    bool startRendering() override;
    void stopRendering() override;

    bool pushRendererNode(CSJGLRendererNodeBase* rendererNode) override;
    void removeRendererNode(CSJGLRendererNodeBase *rendererNode) override;

protected:
    bool initOpenGL();

    void excuteRender();
    void initRenderDevice();

    void render();
    void composite();

private:
    static std::shared_ptr<CSJGLRenderManagerImpl> spRenderManager;

    RendererNodesArray m_rendererNodes;
    std::thread        m_renderThread;

    int                m_width;
    int                m_height;
    HWND               m_hWnd;

    HANDLE             m_stopEvent;
    HANDLE             m_pauseEvent;

    HANDLE             m_glInitEvent;
    HANDLE             m_glUninitEvent;

    CSJGLWindowInfo   *m_window_info = nullptr;
    CSJGLDeviceInfo    m_device_info;
};

#endif // __CSJGLRENDERMANAGERIMPL_H__