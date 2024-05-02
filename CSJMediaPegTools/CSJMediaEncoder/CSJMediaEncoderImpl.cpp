#include "CSJMediaEncoderImpl.h"

 #define videoEncoderName "libx264"
 #define audioEncoderName "aac"

CSJSharedEncoder CSJMediaEncoder::getEncoderWithType(CSJMediaEncoderType fmtType) {
    return std::make_shared<CSJMediaEncoderImpl>();
}

CSJMediaEncoderImpl::CSJMediaEncoderImpl()
    : m_pCodecCtx(nullptr)
    , m_pCodec(nullptr)
    , m_pEncoderFrame(nullptr) {

}

CSJMediaEncoderImpl::~CSJMediaEncoderImpl() {
    if (m_pCodecCtx) {
        avcodec_free_context(&m_pCodecCtx);
    }
}

void CSJMediaEncoderImpl::setDelegate(CSJMediaEncoder::Delegate *delegate) {
    m_pDelegate = delegate;
}

bool CSJMediaEncoderImpl::initVideoEncoderParam(CSJVideoFormatType fmtType, 
                                                int width, 
                                                int height, 
                                                int fps) {

    AVCodec const *videoCodec = nullptr;
    AVCodecContext *videoCodecCtx = nullptr;

    bool res = false;
    do {
        videoCodec = avcodec_find_encoder_by_name(videoEncoderName);
        if (!videoCodec) {
            // find video decoder failed.
            break;
        }

        videoCodecCtx = avcodec_alloc_context3(videoCodec);
        if (!videoCodecCtx) {
            // alloc codec context failed.
            break;
        }

        // set encoder conetxt params.
        videoCodecCtx->bit_rate = 4000000;
        videoCodecCtx->width = width;
        videoCodecCtx->height = height;
        videoCodecCtx->time_base = AVRational{1, 25};
        videoCodecCtx->framerate = AVRational{25, 1};
        videoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
        videoCodecCtx->gop_size = 10;
        videoCodecCtx->max_b_frames = 1;

        if (videoCodec->id == AV_CODEC_ID_H264) {
            // slow: high compressed rate
            // fast: low compressed rate
            av_opt_set(videoCodecCtx->priv_data, "preset", "slow", 0);
        }

        if (avcodec_open2(videoCodecCtx, videoCodec, NULL) < 0) {
            // open encoder failed.
            break;
        }

        m_pCodecCtx = videoCodecCtx;
        m_pCodec = videoCodec;
        m_encoderIdx = 0;

        res = createVideoEncoderFrame();
        if (!res) {
            // create encoder frame failed.
            break;
        }
    
        res = true;
    } while (false);

    if (!res) {
        if (videoCodecCtx) {
            avcodec_free_context(&videoCodecCtx);
        }
    }

    return res;
}

bool CSJMediaEncoderImpl::initAudioEncodeeParam(CSJAudioFormatType fmtType, 
                                                int channel, 
                                                int sampleRate, 
                                                int bitsPerSample) {
    return false;
}

bool CSJMediaEncoderImpl::encoderData(void* data) {
    if (!data) {
        return false;
    }

    bool res = false;
    if (m_encoderType == CSJMEDIAENCODER_VIDEO) {
        CSJVideoData *videoData = (CSJVideoData *)data;
        encoderVideoData(videoData);
    } else if (m_encoderType == CSJMEDIAENCODER_AUDIO) {
        CSJAudioData *audioData = (CSJAudioData *)data;
        encoderAudioData(audioData);
    }

    return true;
}

void CSJMediaEncoderImpl::encoderVideoData(CSJVideoData * videoData) {
    if (!videoData) {
        return ;
    }

    if (!m_pCodecCtx || !m_pEncoderFrame) {
        return ;
    }

    if (!fillVideoFrame(videoData)) {
        return ;
    }

    int ret = avcodec_send_frame(m_pCodecCtx, m_pEncoderFrame);
    if (ret != 0) {
        // send frame to encoder failed.
        return ;
    }

    AVPacket *packet = av_packet_alloc();
    ret = 0;
    while (ret >= 0) {
        ret = avcodec_receive_packet(m_pCodecCtx, packet);
        if (ret == AVERROR_EOF) {
            break;
        }

        if (ret < 0) {
            break;
        }

        /**
         * invoke delegate function to forward pakcet's data to external.
         */
        if (m_pDelegate) {
            m_pDelegate->onEncoderDataArrived(packet->data, packet->size);
        }

        av_packet_unref(packet);
    }

    av_packet_free(&packet);
}

bool CSJMediaEncoderImpl::createVideoEncoderFrame() {
    if (m_pEncoderFrame) {
        av_frame_free(&m_pEncoderFrame);
    }

    if (!m_pCodecCtx) {
        return false;
    }

    bool res = false;
    do {
        AVFrame *frame = av_frame_alloc();
        if (!frame) {
            // alloc AVFrame failed.
            break;
        }

        frame->width = m_pCodecCtx->width;
        frame->height = m_pCodecCtx->height;
        frame->format = m_pCodecCtx->pix_fmt;

        int ret = av_frame_get_buffer(frame, 32);
        if (ret != 0) {
            break ;
        }
    } while (false);

    if (!res && m_pEncoderFrame) {
        av_frame_free(&m_pEncoderFrame);
    }

    return res;
}

bool CSJMediaEncoderImpl::fillVideoFrame(CSJVideoData * videoData) {
    bool res = false;
    if (!videoData || !m_pEncoderFrame) {
        return res;
    }

    CSJVideoFormatType videoFmt = videoData->getFmtType();
    switch (videoFmt) {
        case CSJVIDEO_FMT_NV12:
        break;
        default:
            res = false;
        break;
    }

    return false;
}

bool CSJMediaEncoderImpl::fillVideoFrameWithNV12(CSJVideoData * videoData) {
    if (!videoData || !m_pEncoderFrame) {
        return false;
    }

    int lengthY = videoData->getWidth() * videoData->getHeight();
    memcpy(m_pEncoderFrame->data[0], videoData->getData(), lengthY);

    uint8_t *startUV = videoData->getData() + lengthY;
    for (int i = 0; i < lengthY / 2; i++) {
        memcpy(m_pEncoderFrame->data[1] + i, startUV + i * 2, 1);
        memcpy(m_pEncoderFrame->data[2] + i, startUV + (i * 2) + 1, 1);
    }

    return true;
}

void CSJMediaEncoderImpl::encoderAudioData(CSJAudioData * audioData) {
}

bool CSJMediaEncoderImpl::createAUdioEncoderFrame() {
    return false;
}
