#include "CSJMediaExtracterImpl.h"

#include <stdio.h>

#include <fstream>
#include <iostream>
#include <sstream>

#ifndef AV_WB32 
#define AV_WB32(p, val) do {                \
            uint32_t d = (val);             \
            ((uint8_t *)(p))[3] = (d);      \
            ((uint8_t *)(p))[2] = (d) >> 8; \
            ((uint8_t *)(p))[1] = (d) >> 16;\
            ((uint8_t *)(p))[0] = (d) >> 24;\
        } while(0)
#endif

#ifndef AV_RB_16
#define AV_RB16(x)                              \
            ((((const uint8_t *)(x))[0] << 8) | \
            ((const uint8_t *)(x))[1])
#endif

CSJMediaExtracter* CSJMediaExtracter::getExtracter(std::string &srcFilePath, 
                                                   std::string& dstFilePath,
                                                   CSJExtracteType type) {
    CSJMediaExtracterImpl *impl = new CSJMediaExtracterImpl(srcFilePath, dstFilePath, type);

    return impl;
}

CSJMediaExtracter* CSJMediaExtracter::getExtracter(CSJExtracteType type) {
    return new CSJMediaExtracterImpl(type);
}

CSJMediaExtracterImpl::CSJMediaExtracterImpl(std::string& srcFilePath, 
                                             std::string& dstFilePath,
                                             CSJExtracteType type)
    : m_srcFilePath(srcFilePath)
    , m_dstFilePath(dstFilePath)
    , m_type(type) {

}

CSJMediaExtracterImpl::CSJMediaExtracterImpl(CSJExtracteType type)
    : m_type(type) {

}

void CSJMediaExtracterImpl::setDelegate(std::shared_ptr<CSJMediaExtracter::Delegate> delegate) {
    m_delegate = delegate;
}

void CSJMediaExtracterImpl::setFilePath(std::string & srcFilePath, std::string & dstFilePath) {
    m_srcFilePath = srcFilePath;
    m_dstFilePath = dstFilePath;
}

bool CSJMediaExtracterImpl::extractData() {
    if (m_dstFilePath.size() == 0 || m_srcFilePath.size() == 0) {
        return false;
    }

    bool ret = false;

    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = NULL;
    }

    switch (m_type) {
    case CSJ_EXTRACTDATA_AUDIO:
        ret = extractAudioData();
        break;
    case CSJ_EXTRACTDATA_VIDEO:
        ret = extractVideoData();
        break;
    default:
        break;
    }

    notifyStatusByDelegate();
    closeFormatCtxForExtracting();

    return ret;
}

bool CSJMediaExtracterImpl::initFormatCtxForExtracting(AVMediaType mediaType) {
    if (m_srcFilePath.size() == 0) {
        return false;
    }

    int ret = 0;
    ret = avformat_open_input(&m_formatCtx, m_srcFilePath.c_str(), NULL, NULL);
    if (ret < 0) {
        // source file open failed.
        return false;
    }

    av_dump_format(m_formatCtx, 0, m_srcFilePath.c_str(), 0);

    m_streamIndex = av_find_best_stream(m_formatCtx, mediaType, -1, 0, NULL, 0);
    if (m_streamIndex < 0) {
        // find audio stream failed.
        avformat_close_input(&m_formatCtx);
        m_formatCtx = NULL;
        return false;
    }

    return true;
}

void CSJMediaExtracterImpl::closeFormatCtxForExtracting() {
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = NULL;
    }
}

void CSJMediaExtracterImpl::generateADTSHeader(char *header, int dataSize) {
    if (!header) {
        return;
    }

    int audio_object_type = 2;
    // TODO: 有个问题，44100采样率对应的索引是4，但是如果sampling_frequency_index为4的话，生成
    // 的aac文件采样率为88200，而索引7对应的是22050，但是设置为7，生成的aac文件采样率是44100，后面
    // 要回来解决这个问题 2024/03/24
    int sampling_frequency_index = 7; 
    int channel_config = 2;
    int adtsLen = dataSize + 7;

    header[0] = 0xff;                                       // syncword: 0xfff                                      高8bits

    header[1] = 0xf0;                                       // syncword: 0xfff                                      低8bits    
    header[1] |= (0 << 3);                                  // MPEG Version:0 for MPEG-4,1 for MPEG-2               1bit
    header[1] |= (0 << 1);                                  // Layer: 0                                             2bits
    header[1] |= 1;                                         // proctection absent: 1, no CRC                        1bit

    header[2] = (audio_object_type - 1) << 6;               // profile: audio_object_type - 1;                      2bits
    header[2] |= (sampling_frequency_index & 0x0f) << 2;    // sampling frequency index: sampling_frequency_index   4bits
    header[2] |= (0 << 1);                                  // private bit: 0                                       1bit
    header[2] |= (channel_config & 0x04) >> 2;              // channel configuration: channel_config                高1bit

    header[3] = (channel_config & 0x03) << 6;               // channel configuration: channel_config                低2bits
    header[3] |= (0 << 5);                                  // original: 0                                          1bit
    header[3] |= (0 << 4);                                  // home: 0                                              1bit
    header[3] |= (0 << 3);                                  // copyright id bit: 0                                  1bit
    header[3] |= (0 << 2);                                  // copyright id start: 0                                1bit
    header[3] |= ((adtsLen & 0x1800) >> 11);                // frame length: value                                  高2bits

    header[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);          // frame length: value                                  中间8bits
    header[5] = (uint8_t)((adtsLen & 0x7) << 5);            // frame length: value                                  低3bits
    header[5] |= 0x1f;                                      // buffer fullness: 0x7ff                               高5bits
    header[6] = 0xfc;
}

bool CSJMediaExtracterImpl::extractAudioData() {
    bool ret = initFormatCtxForExtracting(AVMEDIA_TYPE_AUDIO);
    if (!ret) {
        std::cout << "init avformat context failed." << std::endl;
        return ret;
    }

    FILE *dst_file = fopen(m_dstFilePath.c_str(), "wb");
    if (!dst_file) {
        closeFormatCtxForExtracting();
        return false;
    }

    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(m_formatCtx, packet) >= 0) {
        if (packet->stream_index == m_streamIndex) {
            char header[7];
            memset(header, 0, 7);
            generateADTSHeader(header, packet->size);
            fwrite(header, 1, 7, dst_file);
            fwrite(packet->data, 1, packet->size, dst_file);
            

            float curTimeStamp = packet->pts * av_q2d(m_formatCtx->streams[m_streamIndex]->time_base);
            float curDuration = packet->duration * av_q2d(m_formatCtx->streams[m_streamIndex]->time_base);

            // TODO: compute the extracting progress;
            notifyProgressByDelegate(0.0);
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);

    if (dst_file) {
        fflush(dst_file);
        fclose(dst_file);
    }

    return true;
}

bool CSJMediaExtracterImpl::extractVideoData() {
    bool ret = initFormatCtxForExtracting(AVMEDIA_TYPE_VIDEO);
    if (!ret) {
        std::cout << "init avformat context failed." << std::endl;
        return ret;
    }

    FILE *dst_file = fopen(m_dstFilePath.c_str(), "wb");
    if (!dst_file) {
        closeFormatCtxForExtracting();
        return false;
    }

    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(m_formatCtx, packet) >= 0) {
        if (packet->stream_index == m_streamIndex) {
            
            writePacketData(packet, dst_file);

            float curTimeStamp = packet->pts * av_q2d(m_formatCtx->streams[m_streamIndex]->time_base);
            float curDuration = packet->duration * av_q2d(m_formatCtx->streams[m_streamIndex]->time_base);

            // TODO: compute the extracting progress;
            notifyProgressByDelegate(0.0);
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);

    if (dst_file) {
        fflush(dst_file);
        fclose(dst_file);
    }
    return true;
}

void CSJMediaExtracterImpl::writePacketData(AVPacket * inPkt, FILE * dst_file) {
    if (!m_formatCtx || !dst_file) {
        return;
    }

    // the packet save new packet data and will be writen in the dst_file;
    AVPacket *outPkt = av_packet_alloc();
    // the packet includes sps and pps data;
    AVPacket *spsPpsPkt = av_packet_alloc();

    // the data size of inPkt;
    int bufSize = inPkt->size;
    // the start position of the data in inPkt;
    const uint8_t *buf = inPkt->data;
    // the end position of the data in inPkt;
    const uint8_t *buf_end = buf + bufSize;

    // the packet type;
    uint8_t unit_type;
    // the size of nal unit;
    int32_t nal_size;

    uint32_t cumulSize = 0, i;

    int ret = AVERROR(EINVAL);
    do {
        // the first 4 bytes is the length of the pkt,
        if (buf + 4 > buf_end) {
            // now, the pkt data is wrong;
            break;
        }

        // in the first 4 bytes, the low 2 bytes is the high bytes of nal_size,
        // so we must exchange the low 2 bytes and the high 2 bytes. 
        for (nal_size = 0, i = 0; i < 4; i++) {
            nal_size = (nal_size << 8) | buf[i];
        }

        // jump the nal_size area, pointer the real inPkt data;
        buf += 4;
        // the real inPkt data's first bytes' last five bits is the unit type;
        unit_type = *buf & 0x1f;

        // check the nal_size is valid or not;
        if (nal_size > buf_end - buf || nal_size < 0) {
            break;
        }

        if (unit_type == 5) {
            // now, the packet includes sps/pps data;
            writeSpsPpsPacket(m_formatCtx->streams[m_streamIndex]->codecpar->extradata,
                              m_formatCtx->streams[m_streamIndex]->codecpar->extradata_size,
                              spsPpsPkt,
                              AV_INPUT_BUFFER_PADDING_SIZE);

            if (ret = writeDataToNewPacket(outPkt, spsPpsPkt->data, spsPpsPkt->size, buf, nal_size) < 0) {
                break;
            }
        } else {
            if (ret = writeDataToNewPacket(outPkt, NULL, 0, buf, nal_size) < 0) {
                break;
            }
        }

        int len = fwrite(outPkt->data, 1, outPkt->size, dst_file);
        if (len != outPkt->size) {
            // TODO: writing outPkt data to file accours error.
        }
        fflush(dst_file);
    
    } while (false);

    av_packet_free(&spsPpsPkt);
    av_packet_free(&outPkt);
}

int CSJMediaExtracterImpl::writeSpsPpsPacket(const uint8_t * extraData, const int extraDataSize,
                                              AVPacket * outPkt, int padding) {
    uint16_t unit_size;
    uint64_t total_size = 0;
    uint8_t *out = NULL, unit_nb, sps_done = 0;
    uint8_t sps_seen = 0, pps_seen = 0;
    uint8_t sps_offset = 0, pps_offset = 0;

    // the extraData's first four bytes is the header,
    // so we should move four bytes and get the real data.
    const uint8_t *real_extraData = extraData + 4;

    // every unit need a four bytes header as the following.
    static const uint8_t nalu_header[4] = { 0, 0, 0, 1 };

    // length_size indicates how many bytes are used to save
    // sps unit's size. In general, it uses two bytes.
    int length_size = (*real_extraData++ & 0x3) + 1;

    sps_offset = pps_offset = -1;

    // and the following byte is used to indicate the number of
    // sps units, in general, it only be used the last five bits.
    unit_nb = *real_extraData++ & 0x1f;
    if (!unit_nb) {
        goto pps;
    } else {
        sps_offset = 0;
        sps_seen = 1;
    }

    while (unit_nb--) {
        int err;
        // get current sps unit's size.
        unit_size = AV_RB16(real_extraData);
        // the 4 is the length of the header.
        total_size += unit_size + 4;
        if (total_size > INT_MAX - padding) {
            // the extradata is too big, error
            return AVERROR(EINVAL);
        }

        if (real_extraData + 2 + unit_size > extraData + extraDataSize) {
            // current size over the end, error!
            av_free(out);
            return AVERROR(EINVAL);
        }

        // expanding the out with sps unit's size and padding size;
        if ((err = av_reallocp(&out, total_size + padding)) < 0) {
            return err;
        }

        // copy the header;
        memcpy(out + total_size - unit_size - 4, nalu_header, 4);
        // copy the sps unit;
        memcpy(out + total_size - unit_size, real_extraData + 2, unit_size);
        // move the read_extraData point to next unit's start.
        real_extraData += 2 + unit_size;

pps:
        if (!unit_nb && !sps_done++) {
            // The pps unit's size is a byte, which is different from the sps unit.
            // 疑问：通过下面这个解引用，pps的长度是一个字节表示，那么以上+2的地方是否
            // 会出问题，待验证 2024/03/25.
            unit_nb = *real_extraData++;
            if (unit_nb) {
                pps_offset = total_size;
                pps_seen = 1;
            }
        }
    }

    if (out) {
        memset(out + total_size, 0, padding);
    }

    if (!sps_seen) {
        
    }

    if (!pps_seen) {
    
    }

    outPkt->data = out;
    outPkt->size = total_size;

    return length_size;
}

int CSJMediaExtracterImpl::writeDataToNewPacket(AVPacket * outPkt, 
                                                const uint8_t * spsPpsData, uint32_t spsPpsSize, 
                                                const uint8_t * in, uint32_t inSize) {
    if (!outPkt) {
        return -1;
    }

    uint32_t offset = outPkt->size;
    // the nal unit's beginning maybe 0x000001 or 0x00000001;
    // if the outPkt->size is 0, it indicates that current outPkt 
    // doesn't have sps/pps Data, or the current outPkt has sps/pps
    // data.
    uint8_t nal_header_size = offset ? 3 : 4;

    // Growing current outPkt's size with all the sizes;
    int err = av_grow_packet(outPkt, spsPpsSize + inSize + nal_header_size);
    if (err < 0) {
        return err;
    }

    // copy the sps/pps data into the out packet.
    if (spsPpsData) {
        memcpy(outPkt->data + offset, spsPpsData, spsPpsSize);
    }

    // copy the common data into the new packet.
    memcpy(outPkt->data + offset + spsPpsSize + nal_header_size, in, inSize);

    if (!offset) {
        AV_WB32(outPkt->data + spsPpsSize, 1);
    } else {
        (outPkt->data + offset + spsPpsSize)[0] = 0;
        (outPkt->data + offset + spsPpsSize)[1] = 0;
        (outPkt->data + offset + spsPpsSize)[2] = 1;
    }
}

void CSJMediaExtracterImpl::notifyProgressByDelegate(float progress) {
    std::shared_ptr<CSJMediaExtracter::Delegate> delegate = m_delegate.lock();
    if (delegate) {
        // TODO: compute the progress;
        delegate->notifyProgress(progress);
    }
}

void CSJMediaExtracterImpl::notifyStatusByDelegate() {
    std::shared_ptr<CSJMediaExtracter::Delegate> delegate = m_delegate.lock();
    if (delegate) {
        // TODO: notifies when audio extracting is complete;
        delegate->notifyStatus();
    }
}
