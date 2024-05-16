#include "CSJGLRGBARendererNode.h"

#include <iostream>

#include "CSJCommonTool/CSJCommonTool.h"

/**
 * Default, use vao and vbo init the vertex data and color, if define the USE_VAO
 * 0, provide vertex data and color by attribute, should modify the vertex shader 
 * code to accept the input vertex and color data.
 */

#define USE_VAO 0

//#include "GLFW/glfw3.h"

const float g_trianglePoints[] = {
    // point pos        point color;
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
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

#if (USE_VAO == 1)
    if (!initProgramWithVAO(spProgram)) {
        return false;
    }
#else 
    if (!initProgramWithAttribute(spProgram)) {
        return false;
    }
#endif // USE_VAO
    m_spProgram = spProgram;

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

#if (USE_VAO == 1)
    glBindVertexArray(m_vVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
#else 
    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colorAttr);
    glDrawArrays(GL_TRIANGLES, 0, 3);
#endif // USE_VAO
}

bool CSJGLRGBARenererNode::initProgramWithVAO(CSJSpProgram spProgram) {
    if (!spProgram || !spProgram->programUseable()) {
        return false ;
    }

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

bool CSJGLRGBARenererNode::initProgramWithAttribute(CSJSpProgram spProgram) {
    if (!spProgram || !spProgram->programUseable()) {
        return false;
    }

    m_posAttr = glGetAttribLocation(spProgram->getProgram(), "inPosition");
    m_colorAttr = glGetAttribLocation(spProgram->getProgram(), "inVertexColor");

    glBindAttribLocation(spProgram->getProgram(), m_posAttr, "inPosition");
    glBindAttribLocation(spProgram->getProgram(), m_colorAttr, "inVertexColor");

    static float trianglePoints[] = {
        // point pos        point color;
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, trianglePoints);
    glEnableVertexAttribArray(m_posAttr);

    static float triangleColors[] = {
        // point color;
        1.0f, 0.2f, 0.0f,
        0.6f, 1.0f, 0.0f,
        0.0f, 0.3f, 1.0f
    };

    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, GL_FALSE, 0, triangleColors);
    glEnableVertexAttribArray(m_colorAttr);

    return true;
}
