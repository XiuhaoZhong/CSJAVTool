#ifndef __CSJGLRGBARENDERERNODE_H__
#define __CSJGLRGBARENDERERNODE_H__

#include "CSJGLRendererNode.h"

class CSJGLRGBARenererNode : public CSJGLRendererNodeBase {
public:
    CSJGLRGBARenererNode();
    ~CSJGLRGBARenererNode();

    bool init() override;
    void setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) override;
    void updateRenderPos(int width, int height) override;
    void draw() override;

private:
    CSJSpFrameBuffer m_spDefaultFramebuffer = nullptr;
    CSJSpProgram     m_spProgram            = nullptr;

    GLuint           m_vVao;
    GLuint           m_vVbo;
    
};

#endif // __CSJGLRGBARENDERERNODE_H__