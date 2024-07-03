#ifndef __CSJMEDIAENCODER_H__
#define __CSJMEDIAENCODER_H__

#include <memory>

#include "CSJMediaPegToolDefine.h"
#include "CSJMediaData/CSJMediaData.h"

class CSJMediaEncoder;
using CSJSharedEncoder = std::shared_ptr<CSJMediaEncoder>;

typedef enum {
    CSJMEDIAENCODER_AUDIO = 0,
    CSJMEDIAENCODER_VIDEO,
} CSJMediaEncoderType;

class CSJMEDIAPEGTOOL_API CSJMediaEncoder {
public:
    CSJMediaEncoder() {};
    virtual ~CSJMediaEncoder() {};

    /**
     * @brief This is a delegate that notify status to user,
     *        including encoded data after every encoderData
     *        invoked.
     */
    class Delegate {
    public:
        Delegate() = default;
        ~Delegate() = default;

        virtual void onEncoderDataArrived(uint8_t *data, int dataSize) = 0;
    };

    static CSJSharedEncoder getEncoderWithType(CSJMediaEncoderType encoderType);

    virtual void setDelegate(CSJMediaEncoder::Delegate *delegate) = 0;

    /**
     * @brief inilitialize the video encoder param.
     * 
     * @param[in] fmtType   the video format that users want. 
     * @param[in] width     the width of the video.
     * @param[in] height    the height of the video.
     * @param[in] fps       frames per second, default value is 25.
     *
     * @return  return ture if success, or return false.
     */
    virtual bool initVideoEncoderParam(CSJVideoFormatType fmtType, 
                                       int width, 
                                       int height,
                                       int fps = 25) = 0;

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
    virtual bool initAudioEncodeeParam(CSJAudioFormatType fmtType,
                                       int channel,
                                       int sampleRate,
                                       int bitsPerSample) = 0;

    /**
     * @brief encode meida data. If you want to stop encode,  pass
     *        data with NULL.
     *
     * @param[in] data  the media data that will be encode, which is
     *                  a CSJAudioData or CSJVideoData, the encoder
     *                  will translate it to a correct object according
     *                  to CSJMediaEncoderType.
     *
     * @return  return true if success, or return false;
     */
    virtual bool encoderData(void* data) = 0;

};

#endif // __CSJMEDIAENCODER_H__