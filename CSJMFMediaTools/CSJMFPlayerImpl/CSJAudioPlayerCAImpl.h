#ifndef __CSJAUDIOPLAYERCAIMPLH__
#define __CSJAUDIOPLAYERCAIMPL_H__

#include "CSJMFPlayer/CSJAudioPlayer.h"

#include <Audioclient.h>
#include <atlcomcli.h>

#include <thread>

class CSJAudioPlayerCAImpl : public CSJAudioPlayer {
public:
    CSJAudioPlayerCAImpl();
    ~CSJAudioPlayerCAImpl();

    /************************************************************************/
    /*                                                                      */
    /************************************************************************/
    void setFilePath(std::string& filePath) override;
    void setPlayMode(CSJAudioPlayMode mode) override;
    void setDelegate(CSJAudioPlayer::Delegate *delegate) override;

    void configAudioPlayer(DWORD& nChannel, DWORD nSampleRate, DWORD nBitsPerSample) override;

    void play() override;
    void pause() override;
    void resume() override;
    void stop() override;

    bool isStop() override;
    bool isPlaying() override;
    bool isPause() override;

    bool initPlayer() override;

protected:
    bool initCAPlayer();
    void startPlay();

    void fillTestData(UINT32 frames, BYTE *pData);

private:
    CComPtr<IAudioClient>        m_pAudioClient       = nullptr;
    CComPtr<IAudioRenderClient>  m_pAudioRenderClient = nullptr;

    UINT32              m_bufferSize;
    std::thread         m_playerThread;

    HANDLE              m_pSingalEvent       = nullptr;
    HANDLE              m_pStopEvent         = nullptr;

    CSJAudioPlayer::Delegate *m_delegate     = nullptr;
};

#endif // __CSJAUDIOPLAYERCAIMPL_H__