#include "CSJAudioPlayerDSImpl.h"

CSJAudioPlayerDSImpl::CSJAudioPlayerDSImpl()
    : m_nChannel(0)
    , m_nSampleRate(0)
    , m_nBitsPerSample(0)
    , m_playerStatus(CSJAUDIOPLAYER_STOP)
    , m_delegate(nullptr) {

}

CSJAudioPlayerDSImpl::~CSJAudioPlayerDSImpl() {
    
}

void CSJAudioPlayerDSImpl::setDelegate(CSJAudioPlayer::Delegate * delegate) {
    m_delegate = delegate;
}

void CSJAudioPlayerDSImpl::setFilePath(std::string& filePath) {
    m_filePath = filePath;
}

void CSJAudioPlayerDSImpl::setPlayMode(CSJAudioPlayMode mode) {

    WAVEFORMATEX waveFmt;
    waveFmt.wFormatTag = WAVE_FORMAT_PAC;
}

void CSJAudioPlayerDSImpl::configAudioPlayer(DWORD & nChannel, DWORD nSampleRate, DWORD nBitsPerSample) {
    m_nChannel = nChannel;
    m_nSampleRate = nSampleRate;
    m_nBitsPerSample = nBitsPerSample;
}

void CSJAudioPlayerDSImpl::play() {
    if (isPlaying()) {
        return ;
    }

    m_needStop = false;
    m_needPause = false;

    m_playThread = std::thread(&CSJAudioPlayerDSImpl::playAudio, this);
}

void CSJAudioPlayerDSImpl::pause() {
}

void CSJAudioPlayerDSImpl::resume() {
}

void CSJAudioPlayerDSImpl::stop() {
}

bool CSJAudioPlayerDSImpl::isStop() {
    return m_playerStatus == CSJAUDIOPLAYER_STOP;
}

bool CSJAudioPlayerDSImpl::isPlaying() {
    return m_playerStatus == CSJAUDIOPLAYER_PLAYING;
}

bool CSJAudioPlayerDSImpl::isPause() {
    return m_playerStatus == CSJAUDIOPLAYER_PAUSE;
}

void CSJAudioPlayerDSImpl::playAudio() {
    LPDIRECTSOUND8 pDSound = nullptr;
    HRESULT hr = S_OK;
    
    do {
        hr = DirectSoundCreate8(nullptr, &pDSound, nullptr);
        if (FAILED(hr)) {
            return;
        }

        hr = pDSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_NORMAL);
        if (FAILED(hr)) {
            return;
        }

        WAVEFORMATEX waveFmt;
        waveFmt.wFormatTag = WAVE_FORMAT_PCM;
        waveFmt.nChannels = m_nChannel;
        waveFmt.nSamplesPerSec = m_nSampleRate;
        waveFmt.wBitsPerSample = 16;//m_nBitsPerSample;
        waveFmt.nBlockAlign = waveFmt.nChannels * waveFmt.wBitsPerSample / 8;
        waveFmt.nAvgBytesPerSec = waveFmt.nSamplesPerSec * waveFmt.nBlockAlign;
        waveFmt.cbSize = 0;

        // Create the audio buffer.
        DSBUFFERDESC dsBufferDesc;
        ZeroMemory(&dsBufferDesc, sizeof(DSBUFFERDESC));
        dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
        dsBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
        dsBufferDesc.dwBufferBytes = 0;
        dsBufferDesc.lpwfxFormat = &waveFmt;
        LPDIRECTSOUNDBUFFER pDSBuffer = nullptr;

        hr = pDSound->CreateSoundBuffer(&dsBufferDesc, &pDSBuffer, nullptr);
        if (FAILED(hr)) {
            break;
        }

        LPVOID pBuffer = nullptr;
        DWORD bufferSize = 0;
        hr = pDSBuffer->Lock(0, 0, &pBuffer, &bufferSize, 0, 0, 0);
        if (FAILED(hr)) {
            break;
        }

        CSJMFAudioData audioData;
        if (m_delegate) {
            m_delegate->getSampleFromMemory(audioData);
        }

        if (audioData.getLength() > 0) {
          
        }

        // TODO: copy audio data into pBuffer.
        pDSBuffer->Unlock(pBuffer, bufferSize, nullptr, 0);

        // play audio.
        hr = pDSBuffer->Play(0, 0, 0);
        if (FAILED(hr)) {
            break;
        }

        DWORD status = 0;
        while (true) {
            if (m_needStop) {
                break;
            }

            if (m_needPause) {
                break;
            }

            hr = pDSBuffer->GetStatus(&status);
            if (FAILED(hr)) {
                break;
            }

            if (!(status & DSBSTATUS_PLAYING)) {
                break;
            }
        }

    } while (FALSE);

    pDSound->Release();
}
