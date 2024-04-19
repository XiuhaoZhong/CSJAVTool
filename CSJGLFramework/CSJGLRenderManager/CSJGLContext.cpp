#include "CSJGLContext.h"

#include <iostream>

#include "gl/glew.h"
#include "gl/wglew.h"

#define DUMMY_WNDCLASS L"Dummy GL Window Class"
static const wchar_t *dummy_window_class = L"GLDummyWindow";
static bool registered_dummy_window_class = false;

static inline bool gl_getpixelformat(HDC hdc, int *format, PIXELFORMATDESCRIPTOR *pfd) {
    if (!format) {
        return false;
    }

    // Specify the important attributes we care about;
    int pixAttribs[] = {
        WGL_SUPPORT_OPENGL_ARB, 1, // Must support OGL redering; 
        WGL_DRAW_TO_WINDOW_ARB, 1, // pf that can run a window;
        WGL_ACCELERATION_ARB,
        WGL_FULL_ACCELERATION_ARB,
        WGL_DOUBLE_BUFFER_ARB,
        GL_TRUE,
        WGL_COLOR_BITS_ARB, 32, // 8 bits of red precision in window;
        WGL_DEPTH_BITS_ARB, 0,  // 8 bits of green precision in window;
        WGL_STENCIL_BITS_ARB, 0, // 16 bits of depth precision for window;
        WGL_PIXEL_TYPE_ARB,
        WGL_TYPE_RGBA_ARB, // pf should be RGBA type;
        0
    };

    int nPixCount = 0;
    BOOL success = wglChoosePixelFormatARB(hdc, &pixAttribs[0], NULL, 1, format, (UINT*)&nPixCount);

    if (!success || !nPixCount) {
        *format = 0;
    }

    if (!DescribePixelFormat(hdc, *format, sizeof(*pfd), pfd)) {
        return false;
    }

    return true;
}

static inline bool gl_setpixelformat(HDC hdc, int format, PIXELFORMATDESCRIPTOR *pfd) {
    if (!SetPixelFormat(hdc, format, pfd)) {
        return false;
    }

    return true;
}

static void gl_windowinfo_destroy(CSJGLWindowInfo *wi) {
    if (wi) {
        if (wi->hdc) {
            ReleaseDC(wi->hwnd, wi->hdc);
        }

        free(wi);
        memset(wi, 0, sizeof(CSJGLWindowInfo));
    }
}

static void gl_platform_destroy(CSJGLPlatformInfo *plat) {
    if (!plat) {
        return;
    }

    if (plat->hrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(plat->hrc);
    }

    if (plat->windowInfo.hdc) {
        ReleaseDC(plat->windowInfo.hwnd, plat->windowInfo.hdc);
    }

    if (plat->windowInfo.hwnd) {
        DestroyWindow(plat->windowInfo.hwnd);
    }

    free(plat);
}

CSJGLContext* CSJGLContext::getCSJGLContext() {
    static CSJGLContext instance;
    return &instance;
}

CSJGLWindowInfo* CSJGLContext::createWindowInfo(HWND hwnd) {
    CSJGLWindowInfo *wi = (CSJGLWindowInfo*)malloc(sizeof(CSJGLWindowInfo));
    wi->hwnd = hwnd;
    wi->hdc = GetDC(wi->hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    int pixel_format = 0;

    if (!wi) {
        return NULL;
    }

    if (!gl_getpixelformat(wi->hdc, &pixel_format, &pfd)) {
        goto fail;
    }

    if (!gl_setpixelformat(wi->hdc, pixel_format, &pfd)) {
        goto fail;
    }

    return wi;

fail:
    gl_windowinfo_destroy(wi);
    return NULL;
    return nullptr;
}

void CSJGLContext::destoryWindowInfo(CSJGLWindowInfo * platformContext) {
    gl_windowinfo_destroy(platformContext);
}

void CSJGLContext::destroyDeviceInfo(CSJGLDeviceInfo & deviceInfo) {
    if (deviceInfo.platInfo) {
        DestroyWindow(deviceInfo.platInfo->windowInfo.hwnd);
        wglDeleteContext(deviceInfo.platInfo->hrc);
    }
}

CSJGLContext::CSJGLContext() {

}

CSJGLContext::~CSJGLContext() {

}

CSJGLPlatformInfo* CSJGLContext::initGLContext() {
    CSJGLPlatformInfo *plat = (CSJGLPlatformInfo *)malloc(sizeof(CSJGLPlatformInfo));
    DummyContext dummyCtx;

    int pixel_format;
    PIXELFORMATDESCRIPTOR pfd;

    bool res = false;
    do {
        if (!createDummyContext(&dummyCtx)) {
            break;
        }

        if (glewInit() != 0) {
            break;
        }

        if (!registerDummyClass()) {
            break;
        }

        if (!createDummyWindow(plat)) {
            break;
        }

        if (!gl_getpixelformat(dummyCtx.hdc, &pixel_format, &pfd)) {
            break;
        }
        freeDummyContext(&dummyCtx);

        if (!gl_setpixelformat(plat->windowInfo.hdc, pixel_format, &pfd)) {
            break;
        }

        plat->hrc = initBasicContext(plat->windowInfo.hdc);
        if (!plat->hrc) {
            break;
        }
        
        res = true;
    } while (FALSE);

    if (!res) {
        if (plat) {
            gl_platform_destroy(plat);
            plat = NULL;
        }

        freeDummyContext(&dummyCtx);
    }

    return plat;
}

void CSJGLContext::unInitGLContext() {
    wglMakeCurrent(NULL, NULL);
    //wglDeleteContext(m_platContext.hrc);
    //ReleaseDC(m_platContext.hwnd, m_platContext.hdc);
}

bool CSJGLContext::enterGLContext(CSJGLDeviceInfo *deviceInfo) {
    if (!deviceInfo) {
        return false;
    }

    /**
     * Incase the cur_swap_windowInfo's hdc is NULL.
     */
    HDC hdc = deviceInfo->platInfo->windowInfo.hdc;
    if (deviceInfo->cur_swap_windowInfo) {
        hdc = deviceInfo->cur_swap_windowInfo->hdc;
    }

    if (!wglMakeCurrent(hdc, deviceInfo->platInfo->hrc)) {
        //DWORD errCode = GetLastError();
        return false;
    }

    return true;
}

void CSJGLContext::swapLayerBuffer(CSJGLDeviceInfo *deviceInfo) {
    if (!deviceInfo) {
        return ;
    }
    if (!deviceInfo->cur_swap_windowInfo) {
        return ;
    }
    wglSwapLayerBuffers(deviceInfo->cur_swap_windowInfo->hdc, WGL_SWAP_MAIN_PLANE);
}

void CSJGLContext::leaveGLContext(CSJGLDeviceInfo *deviceInfo) {
    wglMakeCurrent(NULL, NULL);
}

bool CSJGLContext::createDummyContext(DummyContext * dummyCtx) {
    WNDCLASS wc;
    if (registered_dummy_window_class) {
        return true;
    }

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_OWNDC;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = dummy_window_class;

    if (!RegisterClass(&wc)) {
        // couldn't create dummy window class.
        return false;
    }

    HWND hwnd = CreateWindowEx(0, dummy_window_class, L"Dummy GL Window",
                               WS_POPUP,
                               0, 0, 2, 2,
                               NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!hwnd) {
        return false;
    }

    dummyCtx->hwnd = hwnd;
    dummyCtx->hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

    int format_index = ChoosePixelFormat(dummyCtx->hdc, &pfd);
    if (!format_index) {
        return false;
    }

    if (!SetPixelFormat(dummyCtx->hdc, format_index, &pfd)) {
        return false;
    }

    dummyCtx->hrc = initBasicContext(dummyCtx->hdc);
    if (!dummyCtx->hrc) {
        return false;
    }

    return true;
}

bool CSJGLContext::registerDummyClass() {
    static bool created = false;

    WNDCLASS wc = { 0 };
    wc.style = CS_OWNDC;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = (WNDPROC)DefWindowProc;
    wc.lpszClassName = DUMMY_WNDCLASS;

    if (created) {
        return true;
    }

    if (!RegisterClass(&wc)) {
        return false;
    }
    created = true;

    return true;
}

bool CSJGLContext::createDummyWindow(CSJGLPlatformInfo * platformCtx) {
    platformCtx->windowInfo.hwnd = CreateWindowEx(0,
                                                  DUMMY_WNDCLASS,
                                                  L"",
                                                  WS_POPUP,
                                                  0, 0, 1, 1,
                                                  NULL,
                                                  NULL,
                                                  GetModuleHandle(NULL),
                                                  NULL);

    if (!platformCtx->windowInfo.hwnd) {
        return false;
    }

    platformCtx->windowInfo.hdc = GetDC(platformCtx->windowInfo.hwnd);
    if (!platformCtx->windowInfo.hdc) {
        return false;
    }

    return true;
}

void CSJGLContext::freeDummyContext(DummyContext * dummyCtx) {
    wglMakeCurrent(NULL, NULL);

    if (dummyCtx->hrc) {
        wglDeleteContext(dummyCtx->hrc);
    }

    if (dummyCtx->hwnd) {
        DestroyWindow(dummyCtx->hwnd);
    }

    memset(dummyCtx, 0, sizeof(DummyContext));
}

HGLRC CSJGLContext::initBasicContext(HDC hdc) {
    HGLRC hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        return NULL;
    }

    if (!wglMakeCurrent(hdc, hglrc)) {
        wglDeleteContext(hglrc);
        return NULL;
    }

    return hglrc;
}
