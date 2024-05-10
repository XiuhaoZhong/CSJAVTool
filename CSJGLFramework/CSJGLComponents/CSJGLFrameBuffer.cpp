#include "CSJGLFrameBuffer.h"

CSJGLFrameBuffer::CSJGLFrameBuffer() {
}

CSJGLFrameBuffer::~CSJGLFrameBuffer() {
}

GLuint CSJGLFrameBuffer::createFrameBuffer(int width, int height, bool onlyTexture) {
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    m_vTexture = texture;

    //  default framebuffer is 0.
    if (onlyTexture) {
        return 0;
    }

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);

    return framebuffer;
}
