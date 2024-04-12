#ifndef __CSJMEDIADATA_H__
#define __CSJMEDIADATA_H__


typedef enum  {
    CSJVIDEO_FMT_NONE = -1,
    CSJVIDEO_FMT_RGB = 0,
    CSJVIDEO_FMT_RGB24,
    CSJVIDEO_FMT_NV12,
    CSJVIDEO_FMT_YUV420P
} CSJVideoFormatType;

typedef enum {
    CSJAUDIO_FMT_NONE = -1,
    CSJAUDIO_FMT_PCM = 0,
    CSJAUDIO_FMT_AAC
} CSJAudioFormatType;

class CSJVideoData {
public:
    CSJVideoData()
        : m_fmtType(CSJVIDEO_FMT_NONE)
        , m_data(nullptr)
        , m_width(0)
        , m_height(0) {
    }

    CSJVideoData(CSJVideoFormatType fmtType, uint8_t* data, int width, int height)
        : m_fmtType(fmtType), m_data(data), m_width(width), m_height(height) {
    
    }
    
    ~CSJVideoData() {
        if (m_data) {
            delete m_data;
            m_data = nullptr;
        }
    }

    CSJVideoData(const CSJVideoData && videoData)
        : m_fmtType(videoData.m_fmtType)
        , m_data(videoData.m_data)
        , m_width(videoData.m_width)
        , m_height(videoData.m_height) {

        videoData.m_fmtType = CSJVIDEO_FMT_NONE;
        videoData.m_data = nullptr;
        videoData.m_width = 0;
        videoData.m_height = 0;
    }

    CSJVideoData& operator=(const CSJVideoData && videoData) {
        m_fmtType = videoData.m_fmtType;
        m_data = videoData.m_data;
        m_width = videoData.m_width;
        m_height = videoData.m_height;

        videoData.m_fmtType = CSJVIDEO_FMT_NONE;
        videoData.m_data = nullptr;
        videoData.m_width = 0;
        videoData.m_height = 0;
        return *this;
    }

    CSJVideoFormatType getFmtType() const {
        return m_fmtType;
    }

    uint8_t* getData() const {
        return m_data;
    }

    int getWidth() const {
        return m_width;
    }

    int getHeight() const {
        return m_height;
    }

private:
    CSJVideoFormatType m_fmtType;
    uint8_t            *m_data;
    int                m_width;
    int                m_height;
};

class CSJAudioData {
public:
    CSJAudioData()
        : m_fmtType(CSJAUDIO_FMT_NONE)
        , m_data(nullptr)
        , m_sampleRate(0)
        , m_channels(0)
        , m_bitsPerSample(0) {
    }

    CSJAudioData(CSJAudioFormatType fmtType, uint8_t* pdata, 
                 int sampleRate, int channels, int bitsPerSample)
        : m_fmtType(fmtType)
        , m_data(pdata)
        , m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_bitsPerSample(bitsPerSample) {
    
    }

    ~CSJAudioData() {
        if (m_data) {
            delete m_data;
            m_data = nullptr;
        }
    }

    CSJAudioData(const CSJAudioData && audioData)
        : m_fmtType(audioData.m_fmtType)
        , m_data(audioData.m_data)
        , m_sampleRate(audioData.m_sampleRate)
        , m_channels(audioData.m_channels)
        , m_bitsPerSample(audioData.m_bitsPerSample) {
        
        audioData.m_fmtType = CSJAUDIO_FMT_NONE;
        audioData.m_data = nullptr;
        audioData.m_sampleRate = 0;
        audioData.m_channels = 0;
        audioData.m_bitsPerSample = 0;
    }

    CSJAudioData& operator=(const CSJAudioData && audioData) {
        m_fmtType = audioData.m_fmtType;
        m_data = audioData.m_data;
        m_sampleRate = audioData.m_sampleRate;
        m_channels = audioData.m_channels;
        m_bitsPerSample = audioData.m_bitsPerSample;

        audioData.m_fmtType = CSJAUDIO_FMT_NONE;
        audioData.m_data = nullptr;
        audioData.m_sampleRate = 0;
        audioData.m_channels = 0;
        audioData.m_bitsPerSample = 0;

        return *this;
    }

private:
    CSJAudioFormatType m_fmtType;
    uint8_t            *m_data;
    int                m_sampleRate;
    int                m_channels;
    int                m_bitsPerSample;
};

#endif // __CSJMEDIADATA_H__