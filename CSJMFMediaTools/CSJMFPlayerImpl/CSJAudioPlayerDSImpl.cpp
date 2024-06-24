#include "CSJAudioPlayerDSImpl.h"

#include <mmdeviceapi.h>

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

/*
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
        waveFmt.wBitsPerSample = m_nBitsPerSample;
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
*/

void CSJAudioPlayerDSImpl::playAudio() {
    IMFMediaSink *mediaSink = createAudioStreamRenderer();
    if (!mediaSink) {
        return ;
    }

    IMFSinkWriter *sinkWriter = NULL;
    HRESULT hr = MFCreateSinkWriterFromMediaSink(mediaSink, NULL, &sinkWriter);
    if (FAILED(hr)) {
        return ;
    }

    sinkWriter->BeginWriting();

    while (true) {
        sinkWriter->SendStreamTick(0, 1);
        CSJMFAudioData audioData;
        if (m_delegate) {
            m_delegate->getSampleFromMemory(audioData);
        }

        if (audioData.getData()) {
            hr = sinkWriter->WriteSample(0, audioData.getData());
        }
    }

    sinkWriter->Finalize();

    SafeRelease(&sinkWriter);
}

IMFMediaSink* CSJAudioPlayerDSImpl::createAudioStreamRenderer() {
    HRESULT hr = S_OK;

    IMMDeviceEnumerator *pEnum = NULL;      // Audio device enumerator.
    IMMDeviceCollection *pDevices = NULL;   // Audio device collection.
    IMMDevice *pDevice = NULL;              // An audio device.
    IMFAttributes *pAttributes = NULL;      // Attribute store.
    IMFMediaSink *pSink = NULL;             // Streaming audio renderer (SAR)
    LPWSTR wstrID = NULL;                   // Device ID.

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

        // Get ID of the first device in the list.
        hr = pDevices->Item(0, &pDevice);
        if (FAILED(hr)) {
            break;
        }

        hr = pDevice->GetId(&wstrID);
        if (FAILED(hr)) {
            break;
        }

        // Create an attribute store and set the device ID attribute.
        hr = MFCreateAttributes(&pAttributes, 2);
        if (FAILED(hr)) {
            break;
        }

        hr = pAttributes->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID,
                                    wstrID);
        if (FAILED(hr)) {
            break;
        }

        // Create the audio renderer.
        hr = MFCreateAudioRenderer(pAttributes, &pSink);
        if (FAILED(hr)) {
            break;
        }

        pSink->AddRef();
    } while (FALSE);

    SafeRelease(&pEnum);
    SafeRelease(&pDevices);
    SafeRelease(&pDevice);
    SafeRelease(&pAttributes);
    CoTaskMemFree(wstrID);

    return pSink;
}
