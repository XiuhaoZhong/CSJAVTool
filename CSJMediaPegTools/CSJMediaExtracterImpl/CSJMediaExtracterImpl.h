#ifndef __CSJMEDIAEXTRACTERIMPL_H__
#define __CSJMEDIAEXTRACTERIMPL_H__

#include "CSJMediaExtracter.h"

#include "CSJMediaPegHeader.h"

/**
 *  音频流和视频的抽取都已经实现，调试、播放通过，遇到的问题在代码注释中有记录
 *  对H264视频码流和音频aac编码格式都有比较实际的了解
 *  2024/03/26
 */

class CSJMediaExtracterImpl : public CSJMediaExtracter {
public:
    CSJMediaExtracterImpl(std::string& srcFilePath, std::string& dstFilePath, CSJExtracteType type);

    CSJMediaExtracterImpl(CSJExtracteType type);

    virtual void setDelegate(std::shared_ptr<CSJMediaExtracter::Delegate> delegate) override;

    virtual void setFilePath(std::string &srcFilePath, std::string& dstFilePath) override;

    virtual bool extractData() override;

protected:
    /*
    *  Initialize avformat context for extracting.
    */
    bool initFormatCtxForExtracting(AVMediaType mediaType);

    /*
    *  Close format context;
    */
    void closeFormatCtxForExtracting();

    /*
    *  Generate ADTS header;
    */
    void generateADTSHeader(char *header, int dataSize);

    /*
    *  Extracts audio data.
    */
    bool extractAudioData();

    /*
    *  Extracts video data;
    */
    bool extractVideoData();

    /*
    *  Write a packet data into file. 
    */
    void writePacketData(AVPacket *inPkt, FILE *dst_file);

    /*
    *  Write packet with sps and pps.
    */
    int writeSpsPpsPacket(const uint8_t* extraData, const int extraDataSize,
                           AVPacket *outPkt, int padding);

    /*
    *  Write packet data to new packet.
    */
    int writeDataToNewPacket(AVPacket *outPkt,
                             const uint8_t* spsPpsData, uint32_t spsPpsSize,
                             const uint8_t *in, uint32_t inSize);

    /*
    *  Notify extracting progress through the delegate.
    */
    void notifyProgressByDelegate(float progress);

    /*
    *  Notify status throught the delegate.
    */
    void notifyStatusByDelegate();

private:
    std::string     m_srcFilePath;
    std::string     m_dstFilePath;
    CSJExtracteType m_type;

    std::weak_ptr<CSJMediaExtracter::Delegate> m_delegate;

    AVFormatContext *m_formatCtx;
    int              m_streamIndex; // the index of the stream which will be extracted;
};

#endif // __CSJMEDIAEXTRACTERIMPL_H__