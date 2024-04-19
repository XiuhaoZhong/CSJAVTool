#ifndef __CSJGLCONTEXT_H__
#define __CSJGLCONTEXT_H__

#include <windows.h>

typedef struct {
    HWND hwnd;
    HDC  hdc;
} CSJGLWindowInfo;

typedef struct {
    HGLRC hrc;
    CSJGLWindowInfo windowInfo;
} CSJGLPlatformInfo;

typedef struct {
    CSJGLPlatformInfo *platInfo;
    CSJGLWindowInfo   *cur_swap_windowInfo;
} CSJGLDeviceInfo;

/**
 * @brief Dummy window context. used to create a openGL context
 * to get openGL functions. When OpenGL context created, the Dummy
 * window could be destoyed.
 */
typedef struct {
    HWND  hwnd;
    HDC   hdc;
    HGLRC hrc;
} DummyContext;

class CSJGLContext {
public:
    static CSJGLContext* getCSJGLContext();

    static CSJGLWindowInfo *createWindowInfo(HWND hwnd);
    static void destoryWindowInfo(CSJGLWindowInfo *platformContext);

    static void destroyDeviceInfo(CSJGLDeviceInfo &deviceInfo);

    CSJGLContext();
    ~CSJGLContext();
    /**
     * @brief init openGL context.
     *
     * @param[in] the handle of the window.
     *
     * @return success returns true, or return false.
     */
    CSJGLPlatformInfo* initGLContext();

    /**
     * @brief unInit the openGL context.
     */
    void unInitGLContext();

    /**
     * @brief enter the OpenGL Context before rendering.
     *
     * @return success returns true, or returns false.
     */
    bool enterGLContext(CSJGLDeviceInfo *deviceInfo);

    /**
     * @brief exchange presentation buffer.
     */
    void swapLayerBuffer(CSJGLDeviceInfo *deviceInfo);

    /**
     * @brief leave the OpenGL Context after rendering.
     */
    void leaveGLContext(CSJGLDeviceInfo *deviceInfo);

protected:
    bool createDummyContext(DummyContext *dummyCtx);
    bool registerDummyClass();
    bool createDummyWindow(CSJGLPlatformInfo *platformCtx);
    void freeDummyContext(DummyContext *dummyCtx);

    HGLRC initBasicContext(HDC hdc);

private:
    CSJGLWindowInfo m_platContext;
};

#endif // __CSJGLCONTEXT_H__