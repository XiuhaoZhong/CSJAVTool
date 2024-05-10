#include "CSJGLRGBARendererNode.h"

#include "CSJCommonTool/CSJCommonTool.h"

//#include "GLFW/glfw3.h"

//const float g_trianglePoints[] = {
//    // point pos        point color;
//     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
//    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
//     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
//};

const float g_trianglePoints[] = {
    // point pos        point color;
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

CSJGLRGBARenererNode::CSJGLRGBARenererNode() {

}

CSJGLRGBARenererNode::~CSJGLRGBARenererNode() {

}

bool CSJGLRGBARenererNode::init() {
    std::string shaderPath = CSJCommonTool::getResourcePath("resources\\shaders\\rgbaShaders");
    if (shaderPath.size() == 0) {
        return false;
    }

    std::string vertexPath = shaderPath + "\\vertexShader.glsl";
    std::string fragmentPath = shaderPath + "\\fragmentshader.glsl";

    CSJSpProgram spProgram = std::make_shared<CSJGLProgram>(vertexPath, fragmentPath, true);
    if (!spProgram->programUseable()) {
        return false;
    }
    m_spProgram = spProgram;
    
    glGenVertexArrays(1, &m_vVao);
    glGenBuffers(1, &m_vVbo);

    glBindVertexArray(m_vVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_trianglePoints), g_trianglePoints, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    return true;
}

void CSJGLRGBARenererNode::setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) {
    if (m_spDefaultFramebuffer != framebuffer) {
        m_spDefaultFramebuffer = framebuffer;
    }
}

void CSJGLRGBARenererNode::updateRenderPos(int width, int height) {

}

void CSJGLRGBARenererNode::draw() {
    if (!m_spDefaultFramebuffer) {
        return;
    }

    m_spProgram->useProgram();

    glBindVertexArray(m_vVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}