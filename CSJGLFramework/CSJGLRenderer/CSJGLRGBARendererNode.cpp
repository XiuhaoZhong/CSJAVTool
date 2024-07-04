#include "CSJGLRGBARendererNode.h"

#include <iostream>

#include "CSJCommonTool/CSJCommonTool.h"
#include "CSJImageTool/CSJImageTool.h"

/**
 * Default, use vao and vbo init the vertex data and color, if define the USE_VAO
 * 0, provide vertex data and color by attribute, should modify the vertex shader 
 * code to accept the input vertex and color data.
 */

#define USE_VAO 1

//#include "GLFW/glfw3.h"

const float g_trianglePoints[] = {
    // point pos        point color;
    /* 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f*/

     0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f, // Top Right
     0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // Bottom Right
    -0.5f,  0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.0f, 1.0f,	// Top Left
    -0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // Bottom Left
     
};

// 绘制矩形使用顶点索引;
const int g_rectPosIdx[] = {
    0, 1, 3,
    1, 2, 3
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

    glGenTextures(1, &m_imageTex);
    glBindTexture(GL_TEXTURE_2D, m_imageTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0, height = 0, tride = 0;
    unsigned char* image = CSJImageTool::readFromImage("resources/images/awesomeface.png", width, height, tride);
    if (!image) {
    
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    return true;
}

bool CSJGLRGBARenererNode::shouldRender() {
    return true;
}

void CSJGLRGBARenererNode::setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) {
    if (m_spDefaultFramebuffer != framebuffer) {
        m_spDefaultFramebuffer = framebuffer;
    }
}

void CSJGLRGBARenererNode::updateRenderContent(CSJVideoData * videoData) {

}

void CSJGLRGBARenererNode::updateRenderPos(int width, int height) {

}

void CSJGLRGBARenererNode::updateTexture() {
}

void CSJGLRGBARenererNode::draw() {
    if (!m_spDefaultFramebuffer) {
        return;
    }

    m_spProgram->useProgram();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_imageTex);
    glUniform1i(glGetUniformLocation(m_spProgram->getProgram(), "imgTex"), 0);

#if (USE_VAO == 1)
    glBindVertexArray(m_vVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    //glGenBuffers(1, &m_vEbo);

    glBindVertexArray(m_vVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_trianglePoints), g_trianglePoints, GL_STATIC_DRAW);

    /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_rectPosIdx), g_rectPosIdx, GL_STATIC_DRAW);*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // texCood attributes;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
