#include "CSJMediaLiveHandler.h"

CSJMediaLiveHandler::CSJMediaLiveHandler()
    : m_pCapture(nullptr) 
    , m_bCaptureInit(false)
    , m_selCapFormatIndex(0)
    , m_selCapResolutionIndex(0)
    , m_pEncoder(nullptr)
    , m_bEncoderInit(false)
    , m_pRenderWindow(NULL)
    , m_bRendererInit(false) {

}

CSJMediaLiveHandler::~CSJMediaLiveHandler() {

}

bool CSJMediaLiveHandler::initLiveHandler() {
    if (!initCapture()) {
        return false;
    }

    return true;
}

void CSJMediaLiveHandler::selectCaptureFormat(int selIndex) {
    m_selCapFormatIndex = selIndex;
}

void CSJMediaLiveHandler::selectCaptureResolution(int selIndex) {
    m_selCapResolutionIndex = selIndex;
}

void CSJMediaLiveHandler::setRenderWindow(HWND hwnd) {
    m_pRenderWindow = hwnd;
}

bool CSJMediaLiveHandler::startCapture() {
    return false;
}

void CSJMediaLiveHandler::stop() {
}

void CSJMediaLiveHandler::onVideDataArrive() {
}

void CSJMediaLiveHandler::onAudioDataArrive(CSJMFAudioData * audioData) {
}

void CSJMediaLiveHandler::onErrorOccurs() {
}

void CSJMediaLiveHandler::onStatusChanged() {
}

void CSJMediaLiveHandler::onEncoderDataArrived(uint8_t * data, int dataSize) {

}

bool CSJMediaLiveHandler::initCapture() {
    CSJSharedCapture capture = CSJMFCapture::getMFCapture();

    if (!capture->initializeCapture()) {
        return false;
    }

    m_pCapture = capture;

    return true;
}

bool CSJMediaLiveHandler::initEncoder() {
    return false;
}

bool CSJMediaLiveHandler::initRender() {
    return false;
}
