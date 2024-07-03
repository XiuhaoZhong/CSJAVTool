#ifndef __CSJGLRENDERMANAGERIMPL_H__
#define __CSJGLRENDERMANAGERIMPL_H__

#include "CSJGLRenderManager.h"

#include <vector>
#include <thread>
#include <mutex>

#include "CSJGLContext.h"

using RendererNodesArray = std::vector<CSJSharedRendererNode>;

class CSJGLRenderManagerImpl : public CSJGLRenderManager {
public:
    CSJGLRenderManagerImpl();
    ~CSJGLRenderManagerImpl();

    static std::shared_ptr<CSJGLRenderManagerImpl> getInstance();

    bool initGL(HWND hwnd, int width, int height) override;
    void unInitGL() override;
    void setFrameRate(DWORD frameRate) override;

    bool initYUVRenderer(CSJVideoFormatType videoFmt, int width, int height) override;
    void updateYUVData(CSJVideoData &videoData) override;

    bool startRendering() override;
    void updateVideo(uint8_t *videoData, DWORD timestamp) override;
    void stopRendering() override;

    bool pushRendererNode(CSJSharedRendererNode rendererNode) override;
    void removeRendererNode(CSJSharedRendererNode rendererNode) override;

protected:
    bool initOpenGL();

    void excuteRender();
    void initRenderDevice();

    void render();
    void composite();

private:
    static std::shared_ptr<CSJGLRenderManagerImpl> spRenderManager;

    std::mutex         m_RenderArrayMtx;
    RendererNodesArray m_rendererNodes;
    std::thread        m_renderThread;
    CSJSpFrameBuffer   m_spDefaultFramebuffer = nullptr;

    DWORD              m_frameRate;
    int                m_width;
    int                m_height;
    HWND               m_hWnd;

    HANDLE             m_stopEvent;
    HANDLE             m_pauseEvent;

    HANDLE             m_glInitEvent;
    HANDLE             m_glUninitEvent;

    CSJGLWindowInfo   *m_window_info = nullptr;
    CSJGLDeviceInfo    m_device_info;

    std::shared_ptr<CSJGLRendererNodeBase> m_pYUVRenderer = nullptr;
    uint8_t           *m_videoData = nullptr;
};

#endif // __CSJGLRENDERMANAGERIMPL_H__