#ifndef __CSJAVAUDIOHANDLER_H__
#define __CSJAVAUDIOHANDLER_H__

#include <mutex>

#include "CSJMFCapture/CSJMFCapture.h"
#include "CSJAudioPlayer/CSJAudioPlayer.h"

class CSJAVAudioHandler : public CSJAudioPlayer::Delegate,
                          public CSJMFCapture::Delegate {
public:

    CSJAVAudioHandler();
    ~CSJAVAudioHandler();

    /**
    *  Initializes capture and player. 
    */
    bool init();

    /************************************************************************/
    /* Implements from CSJAudioPlayer::Delegate                             */
    /************************************************************************/
    void getSampleFromMemory(CSJMFAudioData &audioData) override;

    /************************************************************************/
    /* Implements from CSJMFCapture::Delegate                               */
    /************************************************************************/
    void onVideDataArrive() override;
    void onAudioDataArrive(CSJMFAudioData *audioData) override;
    void onErrorOccurs() override;
    void onStatusChanged() override;

    void startCapture();

    void testPlayer();

private:
    CSJSharedCapture        m_mediaCapture;
    CSJSharedAudioPlayer    m_audioPlayer;

    std::mutex              m_audioDataMutex;
    CSJMFAudioData          m_audioData;

};

#endif // __CSJAVAUDIOHANDLER_H__

