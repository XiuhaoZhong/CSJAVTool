#ifndef __CSJMEDIAENCODERIMPL_H__
#define __CSJMEDIAENCODERIMPL_H__

#include "CSJMediaEncoder.h"
#include "CSJMediaPegHeader.h"

class CSJMediaEncoderImpl : public CSJMediaEncoder {
public:
    CSJMediaEncoderImpl();
    ~CSJMediaEncoderImpl();

    void setDelegate(CSJMediaEncoder::Delegate *delegate) override;

    /**
     * @brief inilitialize the video encoder param.
     * 
     * @param[in] fmtType   the video format that users want. 
     * @param[in] width     the width of the video.
     * @param[in] height    the height of the audio.
     * @param[in] fps       frames per second, default value is 25.
     *
     * @return  return ture if success, or return false.
     */
    bool initVideoEncoderParam(CSJVideoFormatType fmtType, 
                               int width, 
                               int height,
                               int fps = 25) override;

    /**
     * @brief initialize the audio format encoder param.
     *
     * @param[in] fmtType           the audio format that users want.
     * @param[in] chennel           the channel of the audio.
     * @param[in] sampleRate        the sample rate of the audio
     * @param[in] bitsPerSample     the bits of per smaple.
     *
     * @return return true if success, or return false.
     */
    bool initAudioEncodeeParam(CSJAudioFormatType fmtType,
                               int channel,
                               int sampleRate,
                               int bitsPerSample) override;

    /**
     * @brief encode meida data. If you want to stop encode,  pass
     *        data with NULL.
     *
     * @param[in] data  the media data that will be encode.
     *
     * @return  return true if success, or return false;
     */
    bool encoderData(void* data) override;

protected:
    void encoderVideoData(CSJVideoData *videoData);
    bool createVideoEncoderFrame();
    bool fillVideoFrame(CSJVideoData *videoData);

    bool fillVideoFrameWithNV12(CSJVideoData *videoData);

    void encoderAudioData(CSJAudioData *audioData);
    bool createAUdioEncoderFrame();

private:
    const AVCodec             *m_pCodec;
    AVCodecContext            *m_pCodecCtx;
    AVFrame                   *m_pEncoderFrame;

    int                        m_encoderIdx;

    CSJMediaEncoderType        m_encoderType;
    CSJMediaEncoder::Delegate *m_pDelegate;
};

#endif // __CSJMEDIAENCODER_H__