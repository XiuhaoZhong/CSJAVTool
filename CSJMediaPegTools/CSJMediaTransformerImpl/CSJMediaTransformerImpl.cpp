#include "CSJMediaTransformerImpl.h"

CSJMediaTransformer* CSJMediaTransformer::getTransformer() {
    return new CSJMediaTransformerImpl();
}

CSJMediaTransformer* CSJMediaTransformer::getTransformerWithFileName(std::string& srcFile,
                                                                     std::string& destFile) {
    return new CSJMediaTransformerImpl(srcFile, destFile);
}

CSJMediaTransformerImpl::CSJMediaTransformerImpl() {

}

CSJMediaTransformerImpl::CSJMediaTransformerImpl(std::string & srcFile,
                                                 std::string & destFile) 
    : m_srcFile(srcFile)
    , m_destFile(destFile) {

}

CSJMediaTransformerImpl::~CSJMediaTransformerImpl() {
    unInitFormatContext();
}

void CSJMediaTransformerImpl::setTransformerFiles(std::string srcFile,
                                                  std::string destFile) {
    m_srcFile = srcFile;
    m_destFile = destFile;
}

bool CSJMediaTransformerImpl::transformMedia() {
    if (!initFormatContext()) {
        return false;
    }

    int ret;

    // Create the new streams of the outputformatCtx;
    for (int i = 0; i < m_inputFmtCtx->nb_streams; i++) {
        AVStream *outStream;
        AVStream *inStream = m_inputFmtCtx->streams[i];
        // the parameters of the stream.
        AVCodecParameters *in_codecpar = inStream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            m_streamMapping[i] = -1;
            continue;
        }

        m_streamMapping[i] = i;
        outStream = avformat_new_stream(m_outputFmtCtx, NULL);
        if (!outStream) {
            // create stream failed;
        }

        // copy the input stream parameters.
        ret = avcodec_parameters_copy(outStream->codecpar, in_codecpar);
        if (ret < 0) {
            // copy failed;
        }

        outStream->codecpar->codec_tag = 0;
    }

    const AVOutputFormat *outputFmt = m_outputFmtCtx->oformat;
    if (!(outputFmt->flags & AVFMT_NOFILE)) {
        // Create an AVIOCotext for the m_outputFmtCtx;
        ret = avio_open(&m_outputFmtCtx->pb, m_destFile.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            // create the output AVIOContext failed;
        }
    }

    // Writing header of the m_outputFmtCtx;
    ret = avformat_write_header(m_outputFmtCtx, NULL);
    if (ret < 0) {
        // writing header of the m_outputFormatCtx is failed.
        return false;
    }

    AVPacket *pkt = av_packet_alloc();
    while (true) {
        AVStream *inStream, *outStream;

        ret = av_read_frame(m_inputFmtCtx, pkt);
        if (ret < 0) {
            break;
        }

        inStream = m_inputFmtCtx->streams[pkt->stream_index];
        if (pkt->stream_index >= m_streamNb ||
            m_streamMapping[pkt->stream_index] < 0) {
        
            av_packet_unref(pkt);
            continue;
        }

        pkt->stream_index = m_streamMapping[pkt->stream_index];
        outStream = m_outputFmtCtx->streams[pkt->stream_index];

        pkt->pts = av_rescale_q_rnd(pkt->pts, inStream->time_base, outStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt->dts = av_rescale_q_rnd(pkt->dts, inStream->time_base, outStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt->duration = av_rescale_q(pkt->duration, inStream->time_base, outStream->time_base);
        pkt->pos = -1;

        ret = av_interleaved_write_frame(m_outputFmtCtx, pkt);
        av_packet_unref(pkt);
        if (ret < 0) {
            // write packet error;
            break;
        }  
    }

    av_write_trailer(m_outputFmtCtx);

    unInitFormatContext();
    return true;
}

bool CSJMediaTransformerImpl::initFormatContext() {
    if (m_srcFile.empty() || m_destFile.empty()) {
        return false;
    }

    unInitFormatContext();
    
    bool initSuccess = true;
    do {
        int ret = avformat_open_input(&m_inputFmtCtx, m_srcFile.c_str(), 0, 0);
        if (ret < 0) {
            // input avformat context open failed.
            initSuccess = false;
            break;
        }

        ret = avformat_alloc_output_context2(&m_outputFmtCtx, NULL, NULL, m_destFile.c_str());
        if (ret < 0) {
            initSuccess = false;
            break;
        }

        // save the stream indexes of the input avformat.
        m_streamNb = m_inputFmtCtx->nb_streams;
        //av_realloc_array(m_streamMapping, m_streamNb, sizeof(int));

        m_streamMapping = (int *)av_malloc_array(m_streamNb, sizeof(int));
        if (!m_streamMapping) {
            initSuccess = false;
            break;
        }

    } while (false);
   
    if (!initSuccess) {
        unInitFormatContext();
    }

    return initSuccess;
}

void CSJMediaTransformerImpl::unInitFormatContext() {
    if (m_inputFmtCtx) {
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = NULL;
    }

    if (m_outputFmtCtx) {
        avformat_free_context(m_outputFmtCtx);
        m_outputFmtCtx = NULL;
    }

    if (m_streamMapping) {
        av_free(m_streamMapping);
        m_streamMapping = nullptr;
    }
}
