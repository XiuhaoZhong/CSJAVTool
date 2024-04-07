#ifndef __CSJAUDIOPLAYERDSIMPL_H__
#define __CSJAUDIOPLAYERDSIMPL_H__

#include "CSJAudioPlayer.h"

#include <Windows.h>
#include <dsound.h>
#include <thread>

/**
*  The audio player implementation with DirectShow. 
*/
class CSJAudioPlayerDSImpl : public CSJAudioPlayer {
public:
    CSJAudioPlayerDSImpl();
    ~CSJAudioPlayerDSImpl();

    void setDelegate(CSJAudioPlayer::Delegate *delegate);

    void setFilePath(std::string& filePath) override;
    void setPlayMode(CSJAudioPlayMode mode) override;

    void configAudioPlayer(DWORD& nChannel, DWORD nSampleRate, DWORD nBitsPerSample) override;

    void play() override;
    void pause() override;
    void resume() override;
    void stop() override;

    bool isStop() override;
    bool isPlaying() override;
    bool isPause() override;

protected:
    void playAudio();

private:
    DWORD m_nChannel;
    DWORD m_nSampleRate;
    DWORD m_nBitsPerSample;

    CSJAudioPlayMode            m_playMode;
    CSJAudioPlayerStatus        m_playerStatus;

    std::string                 m_filePath;

    bool                        m_needStop;
    bool                        m_needPause;

    std::thread                 m_playThread;
    CSJAudioPlayer::Delegate    *m_delegate;
};

#endif // __CSJAUDIOPLAYERDSIMPL_H__