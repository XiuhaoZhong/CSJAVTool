#include "CSJGLRenderManagerImpl.h"

#include <iostream>
#include <chrono>

#include "gl/glew.h"

#include "CSJGLRenderer\CSJGLRGBARendererNode.h"
#include "CSJGLRenderer\CSJGLYUVRendererNode.h"

using namespace std::chrono;

static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param) {

    const GLubyte *sourceStr = glGetString(source);
    const GLubyte *typeStr = glGetString(type);
    const GLubyte *severityStr = glGetString(severity);

    //std::string sStr((const unsigned char*)sourceStr);

    printf("%s:%s[%s](%d): %s\n", sourceStr, typeStr,
           severityStr, id, message);

}

std::shared_ptr<CSJGLRenderManagerImpl> CSJGLRenderManagerImpl::spRenderManager = nullptr;

CSJGLRenderManagerImpl::CSJGLRenderManagerImpl()
    : m_width(0)
    , m_height(0) {

    m_stopEvent = CreateEvent(NULL, FALSE, FALSE, L"");
    m_pauseEvent = CreateEvent(NULL, FALSE, FALSE, L"");
    m_glInitEvent = CreateEvent(NULL, FALSE, FALSE, L"");
    m_glUninitEvent = CreateEvent(NULL, FALSE, FALSE, L"");

    m_frameRate = defaultFrameRate;
}

CSJGLRenderManagerImpl::~CSJGLRenderManagerImpl() {
    if (m_stopEvent) {
        CloseHandle(m_stopEvent);
        m_stopEvent = NULL;
    }

    if (m_pauseEvent) {
        CloseHandle(m_pauseEvent);
        m_pauseEvent = NULL;
    }

    if (m_glUninitEvent) {
        CloseHandle(m_glUninitEvent);
        m_glUninitEvent = NULL;
    }

    if (m_rendererNodes.size() > 0) {
        m_rendererNodes.clear();
    }
}

std::shared_ptr<CSJGLRenderManagerImpl> CSJGLRenderManagerImpl::getInstance() {
    if (!spRenderManager) {
        spRenderManager = std::make_shared<CSJGLRenderManagerImpl>();
    }

    return spRenderManager;
}

bool CSJGLRenderManagerImpl::initGL(HWND hwnd, int width, int height) {
    bool res = false;

    m_width = width;
    m_height = height;
    m_hWnd = hwnd;

    m_renderThread = std::thread(&CSJGLRenderManagerImpl::excuteRender, this);

    WaitForSingleObject(m_glInitEvent, INFINITE);
    initRenderDevice();

    CloseHandle(m_glInitEvent);
    m_glInitEvent = NULL;

    return res;
}

void CSJGLRenderManagerImpl::unInitGL() {
   
}

void CSJGLRenderManagerImpl::setFrameRate(DWORD frameRate) {
}

bool CSJGLRenderManagerImpl::initYUVRenderer(CSJVideoFormatType videoFmt, int width, int height) {
    std::shared_ptr<CSJGLRendererNodeBase> yuvRenderer = std::make_shared<CSJGLYUVRendererNode>(videoFmt, width, height);
    /*if (!yuvRenderer->init()) {
        return false;
    }*/

    pushRendererNode(yuvRenderer);
    m_pYUVRenderer = yuvRenderer;
    return true;
}

void CSJGLRenderManagerImpl::updateYUVData(CSJVideoData & videoData) {
    if (!m_pYUVRenderer) {
        return;
    }

    m_pYUVRenderer->updateRenderContent(&videoData);
}

bool CSJGLRenderManagerImpl::startRendering() {
    return true;
}

void CSJGLRenderManagerImpl::updateVideo(uint8_t * videoData, DWORD timestamp) {
    if (!m_videoData) {
        m_videoData = new uint8_t[m_width * m_height * 3 / 2];
    }

    memcpy(m_videoData, videoData, m_width * m_height * 3 / 2);
}

void CSJGLRenderManagerImpl::stopRendering() {
    if (m_stopEvent) {
        SetEvent(m_stopEvent);
    }

    WaitForSingleObject(m_stopEvent, INFINITE);
    CSJGLContext::destoryWindowInfo(m_window_info);
}

bool CSJGLRenderManagerImpl::pushRendererNode(CSJSharedRendererNode rendererNode) {
    std::lock_guard<std::mutex> lockGuard(m_RenderArrayMtx);
    bool isExist = false;
    for (auto node : m_rendererNodes) {
        if (node == rendererNode) {
            isExist = true;
            break;
        }
    }

    if (!isExist) {
        m_rendererNodes.push_back(rendererNode);
    }

    return true;
}

void CSJGLRenderManagerImpl::removeRendererNode(CSJSharedRendererNode rendererNode) {
    std::lock_guard<std::mutex> lockGuard(m_RenderArrayMtx);
}

bool CSJGLRenderManagerImpl::initOpenGL() {
    CSJGLContext *context = CSJGLContext::getCSJGLContext();
    if (!context) {
        return false;
    }

    __try {
        m_device_info.platInfo = context->initGLContext();
        if (m_device_info.platInfo) {
            GLint max_texture_size = 0;
            glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &max_texture_size);
            int major = 0, minor = 0;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);

            return true;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // OpenGL initialize failed!
    }

    return false;
}

void CSJGLRenderManagerImpl::excuteRender() {
    bool openGLInited = initOpenGL();
    if (!openGLInited) {
        return;
    }

    SetEvent(m_glInitEvent);

    m_spDefaultFramebuffer = std::make_shared<CSJGLFrameBuffer>();

    std::shared_ptr<CSJGLRGBARenererNode> rgbaRenderer = std::make_shared<CSJGLRGBARenererNode>();
    if (rgbaRenderer->init()) {
        pushRendererNode(rgbaRenderer);
    }

    /**
     * Start rendering.
     */
    render();
}

void CSJGLRenderManagerImpl::initRenderDevice() {
    m_window_info = CSJGLContext::createWindowInfo(m_hWnd);
}

void CSJGLRenderManagerImpl::render() {
    CSJGLContext *context = CSJGLContext::getCSJGLContext();
    HANDLE waitHandles[2] = { m_stopEvent, m_pauseEvent };

    while (true) {
        DWORD idx = WaitForMultipleObjects(2, waitHandles, FALSE, 0);

        if (idx == 0) {
            break;
        } else if (idx == 1) {
            continue;
        }

        m_device_info.cur_swap_windowInfo = m_window_info;
        if (!context->enterGLContext(&m_device_info)) {
            break;
        }

        int frame_internal = (double)(1000) / m_frameRate;
        steady_clock::time_point beforeRenderPt = steady_clock::now();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //setProjectionMatrix(0, w_, 0, h_, -100.0f, 100.0f);
        //	transform_pipeline_.SetMatrixStacks(model_view_matrix_, projection_matrix_);

        glViewport(0, 0, m_width, m_height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        composite();

        context->swapLayerBuffer(&m_device_info);
        context->leaveGLContext(&m_device_info);

        steady_clock::time_point afterRenderPt = steady_clock::now();
        auto costs_time = std::chrono::duration_cast<std::chrono::milliseconds>(afterRenderPt - beforeRenderPt);
        if (costs_time.count() < frame_internal) {
            std::this_thread::sleep_for(std::chrono::milliseconds(frame_internal - costs_time.count()));
        }
    }

    SetEvent(m_glUninitEvent);
    return ;
}

void CSJGLRenderManagerImpl::composite() {
    std::lock_guard<std::mutex> lockGuard(m_RenderArrayMtx);
    if (m_rendererNodes.empty()) {
        return;
    }

    /**
     * traverse the rendererNodes, and excute its draw function.
     */
    for (auto rendererNode : m_rendererNodes) {
        if (!rendererNode->shouldRender()) {
            continue;
        }

        rendererNode->setDefaultFramebuffer(m_spDefaultFramebuffer);
        rendererNode->updateRenderPos(m_width, m_height);
        rendererNode->updateTexture();
        rendererNode->draw();
    }

}
