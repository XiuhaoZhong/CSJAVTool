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

void CSJMediaLiveHandler::selectCaptureDevice(int selIndex) {
    m_selCapDeviceIndex = selIndex;

    m_selCapFormatIndex = 0;
    m_selCapResolutionIndex = 0;
}

void CSJMediaLiveHandler::selectCaptureFormat(int selIndex) {
    m_selCapFormatIndex = selIndex;

    m_selCapResolutionIndex = 0;
}

void CSJMediaLiveHandler::selectCaptureResolution(int selIndex) {
    m_selCapResolutionIndex = selIndex;
}

void CSJMediaLiveHandler::setRenderWindow(HWND hwnd) {
    m_pRenderWindow = hwnd;
}

bool CSJMediaLiveHandler::startCapture() {
    if (m_pCapture) {
        m_pCapture->start();
    }
    return false;
}

bool CSJMediaLiveHandler::isCapturing() {
    if (m_pCapture) {
        return m_pCapture->isStarted();
    }

    return false;
}

void CSJMediaLiveHandler::stopHandler() {
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

void CSJMediaLiveHandler::getAudioCapDevNames(std::vector<std::wstring>& names) {
    if (m_audioCapInfos.empty()) {
        return ;
    }

    auto it = m_audioCapInfos.begin();
    while (it != m_audioCapInfos.end()) {
        names.emplace_back((*it).deviceIndentity.device_name);
        it++;
    }
}

void CSJMediaLiveHandler::getVideoCapDevNames(std::vector<std::wstring>& names) {
    if (m_videoCapInfos.empty()) {
        return;
    }

    auto it = m_videoCapInfos.begin();
    while (it != m_videoCapInfos.end()) {
        names.emplace_back((*it).deviceIdentity.device_name);
        it++;
    }
}

void CSJMediaLiveHandler::getFormatsWithDevIndex(int devIndex, std::vector<std::wstring>& formats) {
    if (m_videoCapInfos.empty()) {
        return;
    }

    if (m_videoCapInfos[devIndex].formats.empty()) {
        return;
    }
    
    auto it = m_videoCapInfos[devIndex].formats.begin();
    while (it != m_videoCapInfos[devIndex].formats.end()) {
        formats.emplace_back(*it);
        it++;
    }
}

void CSJMediaLiveHandler::getResolutionsWithDevIndexAndFmtIndex(int devIndex,
                                                                int fmtIndex, 
                                                                std::vector<std::wstring>& resolutions) {
    if (m_videoCapInfos.empty()) {
        return;
    }

    if (m_videoCapInfos[devIndex].formats.empty()) {
        return;
    }

    std::wstring curFmt = m_videoCapInfos[devIndex].formats[fmtIndex];
    auto it = m_videoCapInfos[devIndex].fmtInfo[curFmt].begin();
    while (it != m_videoCapInfos[devIndex].fmtInfo[curFmt].end()) {
        std::wstring resolutionString = std::to_wstring((*it).resolution.width) + 
                                        L"x" + 
                                        std::to_wstring((*it).resolution.height);
        resolutions.emplace_back(resolutionString);
        it++;
    }
}

bool CSJMediaLiveHandler::initCapture() {
    CSJSharedCapture capture = CSJMFCapture::getMFCapture();

    if (!capture->initializeCapture()) {
        return false;
    }

    capture->setDelegate(this);
    m_pCapture = capture;

    reloadCaptureInfos();

    return true;
}

bool CSJMediaLiveHandler::initEncoder() {
    return false;
}

bool CSJMediaLiveHandler::initRender() {
    return false;
}

void CSJMediaLiveHandler::reloadCaptureInfos() {
    if (!m_pCapture) {
        return;
    }

    m_videoCapInfos.clear();
    m_audioCapInfos.clear();

    m_videoCapInfos = m_pCapture->getVideoDeviceInfo();
    m_audioCapInfos = m_pCapture->getAudioDeviceInfo();
}
