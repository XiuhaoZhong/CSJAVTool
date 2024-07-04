#include "CSJMediaLiveHandler.h"

#include "CSJGLRenderManager/CSJGLRenderManager.h"

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

    selectFormatByIndex(0);
}

void CSJMediaLiveHandler::selectCaptureFormat(int selIndex) {
    selectFormatByIndex(selIndex);
}

void CSJMediaLiveHandler::selectCaptureResolution(int selIndex) {
    selectCaptureResolution(selIndex);
}

void CSJMediaLiveHandler::setRenderWindow(HWND hwnd) {
    m_pRenderWindow = hwnd;
}

bool CSJMediaLiveHandler::startCapture() {
    if (m_pCapture && !m_pCapture->isStarted()) {
        setCurrentCapturePramaters();
        m_pCapture->start();
    }
    return true;
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

void CSJMediaLiveHandler::onVideoArrive(IMFMediaBuffer* videoData, LONGLONG timeStamp) {
    if (!videoData) {
        return ;
    }

    BYTE *buffer = NULL;
    DWORD maxLen;
    DWORD sampleLen = 0;
    HRESULT hr = videoData->Lock(&buffer, &maxLen, &sampleLen);
    if (FAILED(hr)) {
        return;
    }
     
    CSJVideoData videoFrame(m_curVideoFmt, buffer, m_curWidth, m_curHeight);
    CSJGLRenderManager::getDefaultRenderManager()->updateYUVData(videoFrame);
    videoData->Unlock();
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

void CSJMediaLiveHandler::setCurrentCapturePramaters() {
    CSJVideoDeviceInfo selDeviceInfo = m_videoCapInfos[m_selCapDeviceIndex];
    std::wstring selVideoFmt = selDeviceInfo.formats[m_selCapFormatIndex];
    CSJVideoFmtInfo selFmtInfo = selDeviceInfo.fmtInfo[selVideoFmt][m_selCapResolutionIndex];
    m_curWidth = selFmtInfo.resolution.width;
    m_curHeight = selFmtInfo.resolution.height;
    m_curVideoFmt = string2VideoType(selVideoFmt);

    CSJGLRenderManager::getDefaultRenderManager()->initYUVRenderer(m_curVideoFmt, m_curWidth, m_curHeight);
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

void CSJMediaLiveHandler::selectFormatByIndex(int formatIndex) {
    m_selCapFormatIndex = formatIndex;
    selectResolutionByIndex(0);
}

void CSJMediaLiveHandler::selectResolutionByIndex(int resolutionIndex) {
    m_selCapResolutionIndex = resolutionIndex;
    if (!m_pCapture) {
        return;
    }

    m_pCapture->selectedCamera(m_selCapDeviceIndex, m_selCapFormatIndex, m_selCapResolutionIndex);
}
