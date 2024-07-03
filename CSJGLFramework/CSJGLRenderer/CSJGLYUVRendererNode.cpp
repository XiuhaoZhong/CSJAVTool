#include "CSJGLYUVRendererNode.h"

#include <iostream>

#include "CSJCommonTool/CSJCommonTool.h"

#define WRITE_YUV_TO_FILE 0

CSJGLYUVRendererNode::CSJGLYUVRendererNode() {

}

CSJGLYUVRendererNode::CSJGLYUVRendererNode(CSJVideoFormatType fmt_type, int width, int height)
    : m_fmtType(fmt_type)
    , m_width(width) 
    , m_height(height) {
    initYUVBuffers();

    outYuvFile = NULL;
#if WRITE_YUV_TO_FILE
    fopen_s(&outYuvFile, "YUVRenderer.yuv", "wb+");
#endif
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

    m_spProgram = spProgram;
    return true;
}

bool CSJGLYUVRendererNode::shouldRender() {
    return true;
}

void CSJGLYUVRendererNode::setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) {
    m_spDefaultFramebuffer = framebuffer;
}

void CSJGLYUVRendererNode::updateRenderContent(CSJVideoData * videoData) {
    if (!videoData) {
        return;
    }

    std::lock_guard<std::mutex> lockGuard(m_yuvDataMtx);
    CSJVideoData oldData = std::move(m_videoData);
    m_videoData = std::move(*videoData);

#if WRITE_YUV_TO_FILE
    size_t yLength = m_width * m_height;
    if (outYuvFile) {
        if (m_videoData.getyuvY()) {
            fwrite(m_videoData.getyuvY(), 1, yLength, outYuvFile);
        }

        if (m_videoData.getyuvU()) {
            fwrite(m_videoData.getyuvU(), 1, yLength / 4, outYuvFile);
        }

        if (m_videoData.getyuvV()) {
            fwrite(m_videoData.getyuvV(), 1, yLength / 4, outYuvFile);
        }

        fflush(outYuvFile);
    }
#endif
}

void CSJGLYUVRendererNode::updateRenderPos(int width, int height) {
    if (!m_init) {
        if (!init()) {
            return;
        }

        initTexture();

        static float texturePos[] = {
            0.0f, 1.0f,    // left bottom
            1.0f, 1.0f,    // right bottom
            0.0f, 0.0f,    // left top
            1.0f, 0.0f     // right top
        };

        glBindAttribLocation(m_spProgram->getProgram(), m_textCoorAttr, "texPos");
        glVertexAttribPointer(m_textCoorAttr, 2, GL_FLOAT, GL_FALSE, 0, texturePos);
        glEnableVertexAttribArray(m_textCoorAttr);

        m_init = true;
    }

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

    static float renderCoordinates[] = {
        -vRate, -hRate,// 0.0f,   // left bottom
         vRate, -hRate,//0.0f,   // right bottom
        -vRate,  hRate,// 0.0f,   // left top
         vRate,  hRate//, 0.0f    // right top
    };

    m_spProgram->useProgram();
    glEnableVertexAttribArray(m_posAttr);
    glBindAttribLocation(m_spProgram->getProgram(), m_posAttr, "renderPosition");
    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, renderCoordinates);
}

void CSJGLYUVRendererNode::updateTexture() {
    if (!m_init) {
        return;
    }
    
    std::lock_guard<std::mutex> lockGuard(m_yuvDataMtx);
    if (m_videoData.getyuvY()) {
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_videoData.getyuvY());
    }

    if (m_videoData.getyuvU()) {
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_videoData.getyuvU());
    }

    if (m_videoData.getyuvV()) {
        glBindTexture(GL_TEXTURE_2D, m_texY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_videoData.getyuvV());
    }
}

void CSJGLYUVRendererNode::draw() {
    if (!m_init) {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texY);
    glUniform1i(m_locationUniformY, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texU);
    glUniform1i(m_locationUniformU, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texV);
    glUniform1i(m_locationUniformV, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CSJGLYUVRendererNode::initTexture() {
    glGenTextures(1, &m_texY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &m_texU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &m_texV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &m_rgbTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CSJGLYUVRendererNode::initYUVBuffers() {
    releaseYUVBuffers();

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

void CSJGLYUVRendererNode::fillYUVData(uint8_t *data) {
    if (!data) {
        return;
    }

    switch (m_fmtType) {
    case CSJVIDEO_FMT_YUV420P:
        fillI420Data(data);
        break;
    case CSJVIDEO_FMT_NV12:
        fillNV12Data(data);
        break;
    }
}

void CSJGLYUVRendererNode::fillI420Data(uint8_t * data) {
    if (!data) {
        return;
    }

    size_t yLength = m_width * m_height;
    if (m_yData) { 
        memcpy(m_yData, data, yLength);
    }

    if (m_uData) {
        memcpy(m_uData, data + yLength, yLength / 4);    
    }

    if (m_vData) {
        memcpy(m_vData, data + yLength + yLength / 4, yLength / 4);
    }
}

void CSJGLYUVRendererNode::fillNV12Data(uint8_t * data) {
    if (!data) {
        return;
    }

    size_t yLength = m_width * m_height;
    memcpy(m_yData, data, yLength);

    uint8_t* uvStart = data + yLength;
    for (size_t i = 0; i < yLength / 4; i++) {
        memcpy(m_uData + i, uvStart + 2 * i, 1);
        memcpy(m_vData + i, uvStart + 2 * i + 1, 1);
    }

//#if WRITE_YUV_TO_FILE
//    if (outYuvFile) {
//        fwrite(m_yData, 1, yLength, outYuvFile);
//        fwrite(m_uData, 1, yLength / 4, outYuvFile);
//        fwrite(m_vData, 1, yLength / 4, outYuvFile);
//        fflush(outYuvFile);
//    }
//#endif
}

void CSJGLYUVRendererNode::fillYV12Data(uint8_t * data) {

}

