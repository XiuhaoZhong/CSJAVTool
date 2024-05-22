#ifndef __CSJGLYUVRENDERERNODE_H__
#define __CSJGLYUVRENDERERNODE_H__

#include "CSJGLRendererNode.h"

#include "CSJMediaData.h"

class CSJGLYUVRendererNode : public CSJGLRendererNodeBase {
public:
    CSJGLYUVRendererNode();
    /**
     * @brief construct a yuv renderer.
     *
     * @param[in] fmt_type  the format of the video to rendered.
     * @param[in] width     the width of the video.
     * @param[in] height    the height of the video.
     */
    CSJGLYUVRendererNode(CSJVideoFormatType fmt_type, int width, int height);
    ~CSJGLYUVRendererNode();

    /************************************************************************/
    /* implements CSJGLRendererNodeBase                                     */
    /************************************************************************/
    bool init() override;
    void setDefaultFramebuffer(CSJSpFrameBuffer framebuffer) override;
    void updateRenderPos(int width, int height) override;
    void draw() override;

protected:
    void initYUVBuffers();

    void initYUV420Buffers();
    void initYUV422Buffers();
    void initYUV444Buffers();

    void releaseYUVBuffers();

    void releaseVideoData(uint8_t **data);

    void fillYUVData();
     
private:
    // the video data of yuv planars.
    uint8_t            *m_yData = nullptr;
    uint8_t            *m_uData = nullptr;
    uint8_t            *m_vData = nullptr;

    DataProvider       *m_dataProvider = nullptr;

    int                 m_width;
    int                 m_height;
    CSJVideoFormatType  m_fmtType;

    CSJSpFrameBuffer    m_spDefaultFramebuffer;
    CSJSpProgram        m_spProgram;

    GLint               m_locationUniformY;
    GLint               m_locationUniformU;
    GLint               m_locationUniformV;

    // yuv planar texture uniform locations.
    GLuint              m_texY;
    GLuint              m_texU;
    GLuint              m_texV;

    GLint               m_posAttr;      // render coordinates, (-1, -1) ~ (1, 1).
    GLint               m_textCoorAttr; // texture coordinates, (0, 0) ~ (1, 1).
};

#endif // __CSJGLYUVRENDERERNODE_H__