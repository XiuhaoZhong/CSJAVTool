#include "CSJAVAudioHandler.h"

CSJAVAudioHandler::CSJAVAudioHandler() {

}

CSJAVAudioHandler::~CSJAVAudioHandler() {

}

bool CSJAVAudioHandler::init() {
    m_mediaCapture = CSJMFCapture::getMFCapture();
    m_mediaCapture->setDelegate(this);

    m_audioPlayer = CSJAudioPlayer::getCSJAudioPlayer();
    DWORD channel = 2;
    m_audioPlayer->configAudioPlayer(channel, 48000, 32);
    m_audioPlayer->setDelegate(this);
    return true;
}

void CSJAVAudioHandler::getSampleFromMemory(CSJMFAudioData& audioData) {
    m_audioDataMutex.lock();
    audioData = std::move(m_audioData);
    m_audioDataMutex.unlock();
}

void CSJAVAudioHandler::onVideDataArrive() {
}

void CSJAVAudioHandler::onAudioDataArrive(CSJMFAudioData *audioData) {
    m_audioDataMutex.lock();
    m_audioData = std::move(*audioData);
    m_audioDataMutex.unlock();
}

void CSJAVAudioHandler::onErrorOccurs() {
}

void CSJAVAudioHandler::onStatusChanged() {
}

void CSJAVAudioHandler::startCapture() {
    if (!m_mediaCapture->initializeCapture()) {
        // media foundation capture init failed.
        return;
    }

    m_mediaCapture->selectedCamera(0);
    m_mediaCapture->selectedMicrophone(0);

    m_mediaCapture->startCapture();

    m_audioPlayer->play();
}
