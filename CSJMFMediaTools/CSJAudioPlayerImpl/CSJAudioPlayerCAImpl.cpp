#include "CSJAudioPlayerCAImpl.h"

#include <windows.h>
#include <mmdeviceapi.h>

#include "CSJMFMediaHeader.h"

CSJAudioPlayerCAImpl::CSJAudioPlayerCAImpl() {
    m_pSingalEvent = CreateEvent(NULL, FALSE, FALSE, L"");
    m_pStopEvent = CreateEvent(NULL, FALSE, FALSE, L"");

}

CSJAudioPlayerCAImpl::~CSJAudioPlayerCAImpl() {

}

void CSJAudioPlayerCAImpl::setFilePath(std::string & filePath) {

}

void CSJAudioPlayerCAImpl::setPlayMode(CSJAudioPlayMode mode) {
}

void CSJAudioPlayerCAImpl::setDelegate(CSJAudioPlayer::Delegate * delegate) {
    m_delegate = delegate;
}

void CSJAudioPlayerCAImpl::configAudioPlayer(DWORD & nChannel, DWORD nSampleRate, DWORD nBitsPerSample) {

}

void CSJAudioPlayerCAImpl::play() {
    HRESULT hr = m_pAudioClient->Start();
    if (FAILED(hr)) {
        return ;
    }

    m_playerThread = std::thread(&CSJAudioPlayerCAImpl::startPlay, this);
}

void CSJAudioPlayerCAImpl::pause() {
    m_pAudioClient->Stop();
}

void CSJAudioPlayerCAImpl::resume() {
    play();
}

void CSJAudioPlayerCAImpl::stop() {
    m_pAudioClient->Stop();
}

bool CSJAudioPlayerCAImpl::isStop() {
    return false;
}

bool CSJAudioPlayerCAImpl::isPlaying() {
    return false;
}

bool CSJAudioPlayerCAImpl::isPause() {
    return false;
}

bool CSJAudioPlayerCAImpl::initPlayer() {
    return initCAPlayer();
}

bool CSJAudioPlayerCAImpl::initCAPlayer() {
    IMMDeviceEnumerator *pEnum = NULL;      // Audio device enumerator.
    IMMDeviceCollection *pDevices = NULL;   // Audio device collection.
    IMMDevice *pDevice = NULL;              // An audio device

    bool res = false;
    HRESULT hr = S_OK;
    do {
        // Create the device enumerator.
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                              NULL,
                              CLSCTX_ALL,
                              __uuidof(IMMDeviceEnumerator),
                              (void**) &pEnum);

        if (FAILED(hr)) {
            break;
        }

        // Enumerate the rendering devices.
        hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
        if (FAILED(hr)) {
            break;
        }

        UINT endpointCnt = 0;
        hr = pDevices->GetCount(&endpointCnt);
        if (endpointCnt > 0) {
           
        }

        hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
        if (FAILED(hr)) {
            break;
        }

        hr = pDevice->Activate(IID_IAudioClient,
                               CLSCTX_ALL,
                               NULL,
                               (void**) &m_pAudioClient);
        if (FAILED(hr)) {
            break;
        }

        WAVEFORMATEX *pwfx = NULL;
        hr = m_pAudioClient->GetMixFormat(&pwfx);
        if (FAILED(hr)) {
            break;
        }

        hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                       0,
                                       0,
                                       pwfx,
                                       NULL);
        if (FAILED(hr)) {
            break;
        }

        m_pAudioClient->SetEventHandle(m_pSingalEvent);

        UINT32 bufferFrameCount = 0;
        hr = m_pAudioClient->GetBufferSize(&bufferFrameCount);
        if (FAILED(hr)) {
            break;
        }
        m_bufferSize = bufferFrameCount;

        hr = m_pAudioClient->GetService(IID_IAudioRenderClient,
                                        (void**) &m_pAudioRenderClient);
        if (FAILED(hr)) {
            break;
        }

        BYTE *pData;
        DWORD flags = 0;
        // Grab the entire buffer for the initial fill operation.
        hr = m_pAudioRenderClient->GetBuffer(bufferFrameCount, &pData);
        if (FAILED(hr)) {
            break;
        }

        hr = m_pAudioRenderClient->ReleaseBuffer(bufferFrameCount, flags);
        if (FAILED(hr)) {
            break;
        }

        res = true;
    } while (FALSE);

    SafeRelease(&pEnum);
    SafeRelease(&pDevices);
    SafeRelease(&pDevice);

    return res;
}

void CSJAudioPlayerCAImpl::startPlay() {
    HANDLE signals[2] = { m_pSingalEvent, m_pStopEvent };
    while (true) {
        DWORD eventIdx = WaitForMultipleObjects(2, signals, FALSE, INFINITE);
        if (eventIdx == 0) {
            UINT32 curPadding = 0;
            m_pAudioClient->GetCurrentPadding(&curPadding);
            UINT32 frames = m_bufferSize - curPadding;
            BYTE *pData = NULL;
            m_pAudioRenderClient->GetBuffer(frames, &pData);

            if (m_delegate) {
                m_delegate->fillAudioSample(frames, pData);
            } else {
                fillTestData(frames, pData);
            }

            DWORD flags = 0;
            m_pAudioRenderClient->ReleaseBuffer(frames, flags);
        } else if (eventIdx == 1) {
            break;
        }
    }

    m_pAudioClient->Stop();
}

void CSJAudioPlayerCAImpl::fillTestData(UINT32 frames, BYTE * pData) {
    auto m_time = 0.0;
    auto m_deltaTime = 1.0 / frames;

    int nChannels = 2;
    for (int sample = 0; sample < frames; ++sample) {
        float value = 0.05 * std::sin(5000 * m_time);
        int p = sample * nChannels;
        ((float*) pData)[p] = value;
        ((float*) pData)[p + 1] = value;
        m_time += m_deltaTime;
    }
}
