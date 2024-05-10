#ifndef __CSJGLFRAMEBUFFER_H__
#define __CSJGLFRAMEBUFFER_H__

#include "CSJGLFrameworkDefine.h"

#include <memory>

#include "GL/glew.h"

/**
 * This class is hold a bufferframe object and some texture object are used 
 * during the renderer.
 */
class CSJGLFRAMEWORK_API CSJGLFrameBuffer {
public:
    CSJGLFrameBuffer();
    ~CSJGLFrameBuffer();

    /**
     * @brief create a framebuffer.
     *
     * @param[in] width         the width of the texture.
     * @param[in] height        the height of the texture.
     * @param[in] onlyTexture   wheather only create a texture object, when 
     *                          renderer on the default framebuffer, the value
     *                          should be false.
     *
     * @return the framebuffer which created.
     */
    GLuint createFrameBuffer(int width, int height, bool onlyTexture);

    GLuint getFramebuffer() const {
        return m_vFrameBuffer;
    }

    GLuint getTexture() const {
        return m_vTexture;
    }

private:
    GLuint m_vFrameBuffer = 0;  // default framebuffer is 0.
    GLuint m_vTexture;
};

using CSJSpFrameBuffer = std::shared_ptr<CSJGLFrameBuffer>;

#endif // __CSJGLFRAMEBUFFER_H__
