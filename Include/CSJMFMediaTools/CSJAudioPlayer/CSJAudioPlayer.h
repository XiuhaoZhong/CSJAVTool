#ifndef __CSJAUDIOPLAYER_H__
#define __CSJAUDIOPLAYER_H__

#include "CSJMFMediaToolsDefine.h"

//#include "CSJMFMediaHeader.h"
#include <windows.h>

#include <string>

class CSJAudioPlayer;
using CSJSharedAudioPlayer = std::shared_ptr<CSJAudioPlayer>;

/**
*  Audio player type.
*/
typedef enum {
    CSJAUDIOPLAYER_DSHOW = 0,   // audio player based dshow.
    CSJAUDIOPLAYER_MF,          // audio player based MeidaFoundation.
    CSJAUDIOPLAYER_CA,          // audio player based Core Audio API.
} CSJAudioPlayerTpye;

/**
*  audio play mode. 
*/
typedef enum {
    CSJAUDIOPLAYMODE_MEMORY = 0,    // play audio with memory data.
    CSJAUDIOPLAYMODE_FILE,          // play audio with file.
} CSJAudioPlayMode;

typedef enum {
    CSJAUDIOPLAYER_STOP = 0,
    CSJAUDIOPLAYER_PLAYING,
    CSJAUDIOPLAYER_PAUSE,
} CSJAudioPlayerStatus;

class CSJMFMEDIATOOLS_API CSJAudioPlayer {
public:
    CSJAudioPlayer() = default;
    virtual ~CSJAudioPlayer() {}

    class Delegate {
    public:
        Delegate() = default;
        ~Delegate() = default;

        /**
        *  Get audio data from memory.
        *  
        *  @param audioData[in], represent the audio data will be played.
        */
        virtual void getSampleFromMemory(CSJMFAudioData &audioData) {}

        /**
        *  Write audio data into the buffer.
        *
        *  @param frames[in]  the number of samples need write.
        *  @param pData[in]   the pointer of the buffer to write.
        */
        virtual void fillAudioSample(UINT32 frames, BYTE *pData) {}
    };

    static CSJSharedAudioPlayer getCSJAudioPlayer(CSJAudioPlayerTpye type = CSJAUDIOPLAYER_DSHOW);
    
    virtual void setDelegate(CSJAudioPlayer::Delegate *delegate) = 0;

    virtual bool initPlayer() {
        return true;
    }

    /**
    *  Set the play mode. If the CSJAUDIOPLAYMODE_MEMORY is set, you must implement the 
    *  Delegate::getSampleFromMemory(...) function to provide the audio data, or you must
    *  set the file with setFilePath(...) fucntion.
    */
    virtual void setPlayMode(CSJAudioPlayMode mode) = 0;

    virtual void setFilePath(std::string& filePath) = 0;

    /**
    *  Configures the player.
    * 
    *  @param nChannel       the num of the channels.
    *  @param nSampleRate    the sample rate of the audio.
    *  @param nBitPerSample  the sample's  bits.
    *
    *  @return confiures success returns true, or returns false.
    */
    virtual void configAudioPlayer(DWORD& nChannel, DWORD nSampleRate, DWORD nBitsPerSample) = 0;

    /************************************************************************/
    /* Player operations.                                                   */
    /************************************************************************/
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;

    /************************************************************************/
    /* Get player's status                                                  */
    /************************************************************************/
    virtual bool isPlaying() = 0;
    virtual bool isStop() = 0;
    virtual bool isPause() = 0;
};

#endif //__CSJAUDIOPLAYER_H__