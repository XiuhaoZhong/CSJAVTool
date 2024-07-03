#ifndef __CSJGLRENDERMANAGER_H__
#define __CSJGLRENDERMANAGER_H__

#include "CSJGLFrameworkDefine.h"

#include <windows.h>

#include <memory>

#include "CSJMediaData/CSJMediaData.h"
#include "CSJGLRenderer/CSJGLRendererNode.h"

class CSJGLRenderManager;
using CSJSharedRenderManger = std::shared_ptr<CSJGLRenderManager>;

class CSJGLRendererNodeBase;

#define defaultFrameRate 30

class CSJGLFRAMEWORK_API CSJGLRenderManager {
public:
    CSJGLRenderManager() = default;
    virtual ~CSJGLRenderManager() {};

    static CSJSharedRenderManger getDefaultRenderManager();

    /**
     * @brief initialize the OpenGL.
     *
     * @param[in] hwnd   the handle of the window which will be rendered on.
     * @param[in] width  the width of the render area.
     * @param[in] height the height of the render area.
     *
     * @return success return true, or return false.
     */
    virtual bool initGL(HWND hwnd, int width, int height) = 0;

    /**
     * @brief Set the frame rate of video render. Indicates number of frames per second.
     *        The default frame rate is 30.
     *
     * @param[in] frameRate the fresh rate of video, the video push rate is the samce.
     */
    virtual void setFrameRate(DWORD frameRate) = 0;

    /**
     * @brief unInitialize the OpenGL.
     */
    virtual void unInitGL() = 0;

    /**
     * @brief starts rendering.
     *
     * @return success return true, or return false.
     */
    virtual bool startRendering() = 0;

    /**
     * @brief Initialize a yuv renderer to render video frame.
     *
     * param[in] videoFmt   the pixel format of the video frame.
     * param[in] width      the width of video frame.
     * param[in] height     the height of video frame.
     */
    virtual bool initYUVRenderer(CSJVideoFormatType videoFmt, int width, int height) = 0;

    /**
     * @brief Update the YUV data to render.
     *
     * param[in] videoData  the new YUV video data.
     */
    virtual void updateYUVData(CSJVideoData &videoData) = 0;

    /**
     * @brief update video data.
     * 
     * @param[in] videoData     the video will be renderered.
     * @param[in] timestamp     the timestamp of current video data,
                                if the current timestamp is equal to 
                                last timestamp, so it shouldn't renderer.
     */
    virtual void updateVideo(uint8_t *videoData, DWORD timestamp) = 0;

    /**
     * @brief stop rendering.
     */
    virtual void stopRendering() = 0;

    /**
     * @brief Add a renderer node into the pipeline.
     * 
     * @param[in] rendererNode the renderer node which will be added the tail of 
                               the renderer nodes array.
        
     * @return success return true, or return false.
     */
    virtual bool pushRendererNode(CSJSharedRendererNode rendererNode) = 0;

    /**
     * @brief Remove a renderer node from the pipeline.
     *
     * @param[in] rendererMode the renderer node which will be removed.
     */
    virtual void removeRendererNode(CSJSharedRendererNode rendererNode) = 0;
};

#endif // __CSJGLRENDERMANAGER_H__