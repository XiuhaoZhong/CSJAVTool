#ifndef __CSJGLRENDERERNODE_H__
#define __CSJGLRENDERERNODE_H__

#include "CSJGLFrameworkDefine.h"

#include <memory>

#include "CSJGLComponents/CSJGLProgram.h"
#include "CSJGLComponents/CSJGLFrameBuffer.h"

class CSJGLFRAMEWORK_API CSJGLRendererNodeBase {
public:
    CSJGLRendererNodeBase() {};
    virtual ~CSJGLRendererNodeBase() {};

    /**
     * Provides a seris of interfaces for external class to provide data to
     * renderer node.
     */
    class DataProvider {
    public:
        DataProvider() = default;
        ~DataProvider() = default;

        /**
         * @brief Copy NV12 video data to render node. 
         * 
         * Before renderer node call this function, make sure the parameters are
         * NULL.
         *
         * @param[in] y_data    the Y planar of yuv data.
         * @param[in] u_data    the U planar of yuv data.
         * @param[in] v_data    the V planar of yuv data.
         */
        virtual void copyNV12Data(uint8_t *y_data, uint8_t* u_data, uint8_t* v_data) {}
    };

    /**
     * @brief Initialize the renderer node
     *
     * @return returning true indicates success, or return false.
     */
    virtual bool init() = 0;

    /**
     * @brief Set the default framebuffer.
     * 
     * Renderer node could renderer content to the default framebuffer, or add
     * effects to the default framebuffer's texture. Also, users may renderer
     * thier own content on another framebuffer and then apply the content to 
     * default framebuffer. So, every renderer node should know the default 
     * framebuffer.
     *
     * @param[in] framebuffer   the default framebuffer.
     */
    virtual void setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) = 0;

    /**
     * @brief Update the render area with the size of render target. When drawing 
     *        conetnt to a render device(render window), the content's size maybe
     *        different to the window's size, or only a part of the window, so the 
     *        renderer node should adjust the real drawing size or the real position
     *        in this function.
     *
     * @param[in] width  the width of render window in windows.
     * @param[in] height the height of render window in windows.    
     */
    virtual void updateRenderPos(int width, int height) = 0;

    /**
     * @brief Renderer the content of current renderer node.
     */
    virtual void draw() = 0;
};

using CSJSharedRendererNode = std::shared_ptr<CSJGLRendererNodeBase>;

#endif // __CSJGLRENDERERNODE_H__