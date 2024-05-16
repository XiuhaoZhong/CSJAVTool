#include "CSJGLYUVRendererNode.h"

#include <iostream>

#include "CSJCommonTool/CSJCommonTool.h"

CSJGLYUVRendererNode::CSJGLYUVRendererNode() {
}

CSJGLYUVRendererNode::CSJGLYUVRendererNode(CSJVideoFormatType fmt_type, int width, int height)
    : m_fmtType(fmt_type)
    , m_width(width) 
    , m_height(height) {
    initYUVBuffers();
}

CSJGLYUVRendererNode::~CSJGLYUVRendererNode() {
    releaseYUVBuffers();
}

bool CSJGLYUVRendererNode::init() {

    std::string shaderPath = CSJCommonTool::getResourcePath("resources\\shaders\\yuvShaders");
    if (shaderPath.size() == 0) {
        return false;
    }

    std::string vertexPath = shaderPath + "\\vertexShader.glsl";
    std::string fragmentPath = shaderPath + "\\fragmentshader.glsl";

    CSJSpProgram spProgram = std::make_shared<CSJGLProgram>(vertexPath, fragmentPath, true);
    if (!spProgram->programUseable()) {
        return false;
    }

    spProgram->useProgram();

    std::string uniformName = "texY";
    m_locationUniformY = spProgram->getUniformLocation(uniformName);
    uniformName = "texU";
    m_locationUniformU = spProgram->getUniformLocation(uniformName);
    uniformName = "texV";
    m_locationUniformV = spProgram->getUniformLocation(uniformName);

    std::string attriName = "renderPosition";
    m_posAttr = spProgram->getAttributeLocation(attriName);
    std::string attriName1 = "texPos";
    m_textCoorAttr = spProgram->getAttributeLocation(attriName1);

    // bind texture coordinates data.
    //static float texturePos[] = {
    //    0.0f, 1.0f, 0.0f,   // left bottom
    //    1.0f, 1.0f, 0.0f,   // right bottom
    //    0.0f, 0.0f, 0.0f,   // left top
    //    1.0f, 0.0f, 0.0f    // right top
    //};

    static float texturePos[] = {
    0.0f, 1.0f,    // left bottom
    1.0f, 1.0f,    // right bottom
    0.0f, 0.0f,    // left top
    1.0f, 0.0f     // right top
    };

    glBindAttribLocation(spProgram->getProgram(), m_textCoorAttr, "texPos");
    glVertexAttribPointer(m_textCoorAttr, 3, GL_FLOAT, GL_FALSE, 0, texturePos);
    glEnableVertexAttribArray(m_textCoorAttr);

    m_spProgram = spProgram;
    return true;
}

void CSJGLYUVRendererNode::setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) {
    m_spDefaultFramebuffer = framebuffer;
}

void CSJGLYUVRendererNode::updateRenderPos(int width, int height) {
    float hRate = 1.0f;
    float vRate = 1.0f;
    if ((width == m_width && height == m_height) ||
        width * 1.0 / height == m_width * 1.0 / m_height) {
    
    } else if (m_width == m_height) {
    
    } else if (m_width > m_height) {
        hRate = width * m_height * 1.0 / (m_width * height);
    } else {
        vRate = m_width * height * 1.0 / (width * m_height);
    }

    hRate = 1.0f;
    vRate = 1.0f;
    static float renderCoordinates[] = {
        -vRate, -hRate, 0.0f,   // left bottom
         vRate, -hRate, 0.0f,   // right bottom
        -vRate,  hRate, 0.0f,   // left top
         vRate,  hRate, 0.0f    // right top
    };

    glEnableVertexAttribArray(m_posAttr);
    glBindAttribLocation(m_spProgram->getProgram(), m_posAttr, "renderPosition");
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, renderCoordinates);
}

void CSJGLYUVRendererNode::draw() {
    fillYUVData();

    m_spProgram->useProgram();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texY);
    glUniform1i(m_locationUniformY, 0);
    GLenum error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texU);
    glUniform1i(m_locationUniformU, 1);

    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texV);
    glUniform1i(m_locationUniformV, 2);
    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_textCoorAttr);
    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }
}

void CSJGLYUVRendererNode::initYUVBuffers() {
    switch (m_fmtType) {
    case CSJVIDEO_FMT_NV12:
    case CSJVIDEO_FMT_YUV420P:
        initYUV420Buffers();
        break;
    default:
        break;
    }
}

void CSJGLYUVRendererNode::initYUV420Buffers() {
    if (m_width == 0 || m_height == 0) {
        return ;
    }

    size_t yLength = m_width * m_height;

    m_yData = new uint8_t[yLength];
    memset(m_yData, 0, yLength);

    size_t uvLength = yLength / 4;
    m_uData = new uint8_t[uvLength];
    memset(m_uData, 0, uvLength);

    m_vData = new uint8_t[uvLength];
    memset(m_vData, 0, uvLength);

    GLenum error = glGetError();
    glGenTextures(1, &m_texY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }

    glGenTextures(1, &m_texU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }

    glGenTextures(1, &m_texV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    error = glGetError();
    if (error == 0) {
        std::cout << "" << std::endl;
    }
}

void CSJGLYUVRendererNode::initYUV422Buffers() {
}

void CSJGLYUVRendererNode::initYUV444Buffers() {
}

void CSJGLYUVRendererNode::releaseYUVBuffers() {
    releaseVideoData(&m_yData);
    releaseVideoData(&m_uData);
    releaseVideoData(&m_vData);
}

void CSJGLYUVRendererNode::releaseVideoData(uint8_t ** data) {
    if (!(*data)) {
        return;
    }

    delete[](*data);
    *data = nullptr;
}

void CSJGLYUVRendererNode::fillYUVData() {
    GLenum error = 0;
    size_t yLength = m_width * m_height;
    if (m_yData) {
        memset(m_yData, 99, yLength);
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_yData);
        error = glGetError();
        if (error == 0) {
            std::cout << "" << std::endl;
        }
    }

    size_t uvLength = yLength / 4;
    if (m_uData) {
        memset(m_uData, 66, uvLength);
        glBindTexture(GL_TEXTURE_2D, m_texU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_uData);
        error = glGetError();
        if (error == 0) {
            std::cout << "" << std::endl;
        }
    }
    
    if (m_vData) {
        memset(m_vData, 99, uvLength);
        glBindTexture(GL_TEXTURE_2D, m_texV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_vData);
        error = glGetError();
        if (error == 0) {
            std::cout << "" << std::endl;
        }
    }
}

