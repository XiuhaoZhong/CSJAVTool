#ifndef __CSJGLRGBARENDERERNODE_H__
#define __CSJGLRGBARENDERERNODE_H__

#include "CSJGLRendererNode.h"

class CSJGLRGBARenererNode : public CSJGLRendererNodeBase {
public:
    CSJGLRGBARenererNode();
    ~CSJGLRGBARenererNode();

    bool init() override;
    bool shouldRender() override;
    void setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) override;
    void updateRenderContent(CSJVideoData *videoData) override;
    void updateRenderPos(int width, int height) override;
    void updateTexture() override;
    void draw() override;

protected:
    bool initProgramWithVAO(CSJSpProgram spProgram);
    bool initProgramWithAttribute(CSJSpProgram spProgram);

private:
    CSJSpFrameBuffer m_spDefaultFramebuffer = nullptr;
    CSJSpProgram     m_spProgram            = nullptr;

    GLuint           m_vVao;
    GLuint           m_vVbo;


    GLuint           m_posAttr;
    GLuint           m_colorAttr;
    
};

#endif // __CSJGLRGBARENDERERNODE_H__