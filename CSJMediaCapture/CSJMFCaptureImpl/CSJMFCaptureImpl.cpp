#include "CSJMFCaptureImpl.h"

#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>

#include <iostream>
#include <fstream>

#include "CSJMFCaptureHeader.h"

static std::string SubTypeToString(GUID& subtype) {
    WCHAR buffer[128];
    StringFromGUID2(subtype, buffer, sizeof(buffer));

    std::string res = "";
    std::wstring st(buffer);
    if (st.compare(L"{3231564E-0000-0010-8000-00AA00389B71}") == 0) {
        res = "NV12";
    } else if (st.compare(L"{47504A4D-0000-0010-8000-00AA00389B71}") == 0) {
        res = "MJPG";
    } else if (st.compare(L"{32595559-0000-0010-8000-00AA00389B71}") == 0) {
        res = "YV12";
    } else if (st.compare(L"{00000003-0000-0010-8000-00AA00389B71}") == 0) {
        res = "MFAudioFormat_Float";
    }

    return res;
}

std::string wstring2string(std::wstring &wstr) {
    std::string res;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    res.append(buffer);
    delete[] buffer;
    return res;
}

CSJSharedCapture CSJMFCapture::getMFCapture() {
    return std::make_shared<CSJMFCaptureImpl>();
}

CSJMFCaptureImpl::CSJMFCaptureImpl() {
    m_videoDevicesCnt = 0;
    m_videoDevices = NULL;
    m_szCurCaptureSymlink = NULL;

    m_audioDevicesCnt = 0;
    m_audioDevices = NULL;
    m_szAudioEndpointID = NULL;
    //m_audioCapMS = NULL;

    m_delegate = nullptr;
}

CSJMFCaptureImpl::~CSJMFCaptureImpl() {
    finalize();

    CoTaskMemFree(m_szCurCaptureSymlink);
    m_szCurCaptureSymlink = NULL;
    //SafeRelease(&m_videoCapMS);

    CoTaskMemFree(m_szAudioEndpointID);
    m_szAudioEndpointID = NULL;
}

bool CSJMFCaptureImpl::initializeCapture() {
    loadVideoDeviceInfos();

    loadAudioDeviceInfos();

    /*if (m_videoDevicesCnt == 0 && m_audioDevicesCnt == 0) {
        return false;
    }*/

    return true;
}

CSJMFDeviceList CSJMFCaptureImpl::getVideoCapDevices() {
    return m_videoDevs;
}

CSJMFDeviceList CSJMFCaptureImpl::getAudioCapDevices() {
    return m_audioDevs;
}

void CSJMFCaptureImpl::selectedCamera(int camera_index) {
    WCHAR *symlink;
    if (isSameVideoDevice(camera_index, &symlink)) {
        return ;
    }

    // Free the old device symlink;
    CoTaskMemFree(m_szCurCaptureSymlink);
    // Save the new device symlink;
    m_szCurCaptureSymlink = symlink;

    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return ;
    }

    // 选择摄像头时，如果在采集中，需要切换摄像头
    // TODO: 1、关闭当前视频采集设备
    //       2、重建mediaSession 以及 topology
    //       3、打开新的视频设备
    stopCapture();

    IMFMediaSource **capureMediaSource = NULL;
    HRESULT hr = m_videoDevices[camera_index]->ActivateObject(IID_PPV_ARGS(capureMediaSource));
    if (FAILED(hr)) {
        return;
    }

    // 接下来需要创建输出的MediaSink，需要自定义
    // 然后根据上面的Mediasource和MediaSink，来创建Topology，将MediaSource和MediaSink连接起来
    // 将创建的Topology传递给MediaSession对象，整个采集链路就完成了。
}

void CSJMFCaptureImpl::selectedMicrophone(int microphone_index) {
    WCHAR *endpointID = NULL;
    if (isSameAudioDevice(microphone_index, &endpointID)) {
        return;
    }

    CoTaskMemFree(m_szAudioEndpointID);
    m_szAudioEndpointID = endpointID;

    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return;
    }

    // 选择麦克风时，如果在采集中，需要切换麦克风
    // TODO: 1、关闭当前麦克风设备
    //       2、重建mediaSession 以及 topology
    //       3、打开新的麦克风设备

}

bool CSJMFCaptureImpl::startCapture() {    
    //m_videoCapThread = std::thread(&CSJMFCaptureImpl::startVideoCapWithSourceReader, this);
    m_audioCapThread = std::thread(&CSJMFCaptureImpl::startAudioCapWithSourceReader, this);

    return true;
}

void CSJMFCaptureImpl::pauseCapture() {
    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return;
    }

    m_status = CSJMF_CAPTURE_PAUSE;
}

void CSJMFCaptureImpl::resumeCapture() {
    if (m_status != CSJMF_CAPTURE_PAUSE) {
        return ;
    }

    m_status = CSJMF_CAPTURE_CAPTURING;
}

void CSJMFCaptureImpl::stopCapture() {
    if (m_status == CSJMF_CAPTURE_STOP) {
        return ;
    }

    m_isStop = true;
}

void CSJMFCaptureImpl::setDelegate(CSJMFCapture::Delegate * delegate) {
    m_delegate = delegate;
}

void CSJMFCaptureImpl::startVideoCapWithSourceReader() {
    IMFMediaSource *mediaSource = createVideoCaptureSourceWithSymlink();

    if (!mediaSource) {
        return ;
    }

    IMFMediaType *mediaType = getSelectedVideoMediaType(mediaSource);
    if (!mediaType) {
        return ;
    }

    IMFSourceReader *pReader = NULL;
    HRESULT hr = S_OK;

    // 已经验证，此方法好用，可以读取到摄像头数据
    hr = MFCreateSourceReaderFromMediaSource(mediaSource, NULL, &pReader);
    if (SUCCEEDED(hr)) {
        // 设置输出格式
        hr = pReader->SetCurrentMediaType((DWORD) MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, mediaType);
        if (SUCCEEDED(hr)) {
            m_isStop = false;
            m_status = CSJMF_CAPTURE_CAPTURING;

            // 读取帧
            std::ofstream outFile;
            outFile.open("video_capture.txt", std::ios::out);

            IMFSample *pBuffer = NULL;
            DWORD dwStreamIndex, dwStreamFlags;
            LONGLONG llTimeStamp;
            while (true) {
                if (m_isStop) {
                    break;
                }

                if (outFile.is_open()) {
                    outFile << "start to ReadSample" << std::endl;
                }

                hr = pReader->ReadSample((DWORD) MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwStreamIndex, &dwStreamFlags, &llTimeStamp, &pBuffer);
                if (SUCCEEDED(hr)) {
                    // 第一帧可能是为空，需要做一个判断;
                    if (pBuffer == NULL) {
                        if (outFile.is_open()) {
                            outFile << "Read a empty sample from camera\n" << std::endl;
                        }
                        continue;
                    }

                    if (outFile.is_open()) {
                        outFile << "Read a sample from camera\n" << std::endl;
                    }
                    DWORD sampleLen = 0;
                    pBuffer->GetBufferCount(&sampleLen);
                    /*if (outFile.is_open()) {
                        outFile << "capture a video, legnth:" << sampleLen << std::endl;
                    }*/

                    IMFMediaBuffer *buf;
                    pBuffer->GetBufferByIndex(0, &buf);
                    if (buf) {

                        BYTE *buffer = NULL;
                        DWORD maxLen;
                        hr = buf->Lock(&buffer, &maxLen, &sampleLen);
                        if (FAILED(hr)) {
                            std::cout << "lock sample failed." << std::endl;
                        }
                    }

                    // TODO: invoke a delegate function to diliver the video data and timestamp to render.

                    SafeRelease(&pBuffer);
                }
            }

            if (outFile.is_open()) {
                outFile << "video capture is exit" << std::endl;
            }
        }
    }

    SafeRelease(&mediaType);
    // pReader release 之后，会调用mediaSource的shutdown()方法
    SafeRelease(&pReader);
    SafeRelease(&mediaSource);
    m_isStop = false;
    m_status = CSJMF_CAPTURE_STOP;
}

void CSJMFCaptureImpl::startAudioCapWithSourceReader() {
    IMFMediaSource *mediaSource = createAudioCaptureSourceWithEndPoint();
    if (!mediaSource) {
        return ;
    }

    IMFMediaType *mediaType = getSelectedAudioMediaType(mediaSource);
    if (!mediaType) {
        return ; 
    }

    IMFSourceReader *pReader = NULL;
    HRESULT hr = S_OK;

    // 已经验证，此方法好用，可以读取到摄像头数据
    hr = MFCreateSourceReaderFromMediaSource(mediaSource, NULL, &pReader);
    if (SUCCEEDED(hr)) {
        // 设置输出格式
        hr = pReader->SetCurrentMediaType((DWORD) MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, mediaType);
        if (SUCCEEDED(hr)) {
            m_isStop = false;
            m_status = CSJMF_CAPTURE_CAPTURING;

            // 读取帧
            while (SUCCEEDED(hr)) {
                if (m_isStop) {
                    break;
                }

                IMFSample *pBuffer = NULL;
                DWORD dwStreamIndex, dwStreamFlags;
                LONGLONG llTimeStamp;

                hr = pReader->ReadSample((DWORD) MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &dwStreamIndex, &dwStreamFlags, &llTimeStamp, &pBuffer);
                if (SUCCEEDED(hr)) {
                    // 第一帧可能是为空，需要做一个判断;
                    if (pBuffer == NULL) {
                        continue;
                    }
                    DWORD sampleLen = 0;
                    pBuffer->GetBufferCount(&sampleLen);

                    IMFMediaBuffer *buf;
                    pBuffer->GetBufferByIndex(0, &buf);
                    if (buf) {

                        BYTE *buffer = NULL;
                        DWORD maxLen;
                        hr = buf->Lock(&buffer, &maxLen, &sampleLen);
                        if (FAILED(hr)) {
                            std::cout << "lock sample failed." << std::endl;
                        }
                    }

                    // TODO: invoke a delegate function to diliver the video data and timestamp to render.

                    SafeRelease(&pBuffer);
                }
            }
        }
    }

    SafeRelease(&mediaType);
    // pReader release 之后，会调用mediaSource的shutdown()方法
    SafeRelease(&pReader);
    SafeRelease(&mediaSource);

}

void CSJMFCaptureImpl::loadVideoDeviceInfos() {
    releaseVideoDeviceInfo();

    IMFAttributes *pAttributes = NULL;
    do {
        HRESULT hr = MFCreateAttributes(&pAttributes, 1);
        if (FAILED(hr)) {
            break;
        }

        hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                                  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        if (FAILED(hr)) {
            break;
        }

        hr = MFEnumDeviceSources(pAttributes, &m_videoDevices, &m_videoDevicesCnt);
        if (FAILED(hr) || m_videoDevicesCnt == 0) {
            break;
        }

        WCHAR *szFriendlyName = NULL;
        UINT32 ccName;
        for (DWORD i = 0; i < m_videoDevicesCnt; i++) {
            // 获取设备的名称，不是唯一，因此不能用来作为判断选择设备的依据.
            hr = m_videoDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                                       &szFriendlyName,
                                                       &ccName);
            if (FAILED(hr)) {
                continue;
            }

            std::wstring devName(szFriendlyName);
            m_videoDevs.push_back(devName);

            WCHAR *devSymlink;
            UINT32 cchSymlink;
            hr = m_videoDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                                       &devSymlink,
                                                       &cchSymlink);

            CSJVideoDeviceInfo deviceInfo;
            deviceInfo.device_name = szFriendlyName;
            deviceInfo.device_symlink = devSymlink;

            IMFMediaSource *pSource = NULL;
            hr = m_videoDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));
            if (FAILED(hr)) {
                break;
            }

            loadVideoMediaSourceInfos(pSource, deviceInfo);
            m_videoDeivceInfos.insert({ deviceInfo.device_symlink, deviceInfo });
            SafeRelease(&pSource);
        }
    } while (FALSE);

    SafeRelease(&pAttributes);
    pAttributes = NULL;
}

void CSJMFCaptureImpl::loadVideoMediaSourceInfos(IMFMediaSource * mediaSource, CSJVideoDeviceInfo& deviceInfo) {
    if (!mediaSource) {
        return ;
    }

    IMFPresentationDescriptor *descriptor;
    HRESULT hr = mediaSource->CreatePresentationDescriptor(&descriptor);
    if (FAILED(hr)) {
        return ;
    }

    DWORD streamCnt = 0;
    hr = descriptor->GetStreamDescriptorCount(&streamCnt);
    if (FAILED(hr)) {
        return ;
    }

    IMFStreamDescriptor *streamDescriptor;
    for (DWORD i = 0; i < streamCnt; i++) {
        BOOL isSelected = FALSE;
        hr = descriptor->GetStreamDescriptorByIndex(i, &isSelected, &streamDescriptor);
        if (FAILED(hr)) {
            continue;
        }

        IMFMediaTypeHandler *typeHandler;
        hr = streamDescriptor->GetMediaTypeHandler(&typeHandler);
        if (FAILED(hr)) {
            continue;
        }

        DWORD mediaTypeCnt;
        hr = typeHandler->GetMediaTypeCount(&mediaTypeCnt);
        if (FAILED(hr)) {
            continue;
        }

        for (DWORD i = 0; i < mediaTypeCnt; i++) {
            IMFMediaType *mediaType;
            hr = typeHandler->GetMediaTypeByIndex(i, &mediaType);
            if (FAILED(hr)) {
                continue;
            }

            BOOL isCompressedFMT = FALSE;
            hr = mediaType->IsCompressedFormat(&isCompressedFMT);
            if (FAILED(hr)) {
                continue;
            }

            if (isCompressedFMT) {
                std::cout << "compressed fmt" << std::endl;
            }

            GUID subtype = { 0 };
            UINT32 frameRate = 0;
            UINT32 denominator = 0;
            UINT32 width = 0, height = 0;
            UINT32 frameRateMin = 0, frameRateMax = 0;

            hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
            hr = MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height);
            hr = MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE, &frameRate, &denominator);
            hr = MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE_RANGE_MIN, &frameRateMin, &denominator);
            hr = MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE_RANGE_MAX, &frameRateMax, &denominator);

            CSJVideoFmtInfo cameraInfo;
            cameraInfo.sub_type = subtype;
            cameraInfo.fmt_name = SubTypeToString(subtype);
            cameraInfo.width = width;
            cameraInfo.height = height;
            cameraInfo.frameRate = frameRate;

            deviceInfo.fmtList.push_back(cameraInfo);
        }
    }
}

void CSJMFCaptureImpl::loadAudioDeviceInfos() {
    releaseAudioDeviceInfo();

    IMFAttributes *pAttributes = NULL;

    do {
        HRESULT hr = MFCreateAttributes(&pAttributes, 1);
        if (FAILED(hr)) {
            break;
        }

        hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                                  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
        if (FAILED(hr)) {
            break;
        }

        hr = MFEnumDeviceSources(pAttributes, &m_audioDevices, &m_audioDevicesCnt);
        if (FAILED(hr) || m_audioDevicesCnt == 0) {
            break;
        }

        for (DWORD i = 0; i < m_audioDevicesCnt; i++) {
            WCHAR *szFriendlyName = NULL;
            UINT32 ccName;
            // 获取设备的名称，不是唯一，因此不能用来作为判断选择设备的依据.
            hr = m_audioDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                                       &szFriendlyName,
                                                       &ccName);
            if (FAILED(hr)) {
                continue;
            }

            std::wstring devName(szFriendlyName);
            m_audioDevs.push_back(devName);

            IMFMediaSource *pSource = NULL;
            hr = m_audioDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));
            if (FAILED(hr)) {
                break;
            }

            WCHAR *devEndPoint;
            UINT32 cchEndPoint;
            hr = m_audioDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID,
                                                       &devEndPoint,
                                                       &cchEndPoint);
            if (FAILED(hr)) {
                continue ;
            }
            
            std::wstring endPointID(devEndPoint);
            CSJAudioDeviceInfo deviceInfo;
            deviceInfo.device_name = devName;
            deviceInfo.device_endPintID = endPointID;
            loadAudioMediaSourceInfos(pSource, deviceInfo);
            m_audioDeviceInfos.insert({ endPointID, deviceInfo });

        }
    } while (FALSE);
    
    SafeRelease(&pAttributes);
    pAttributes = NULL;
}

IMFMediaSource * CSJMFCaptureImpl::createAudioCaptureSourceWithEndPoint() {
    if (!m_szAudioEndpointID) {
        return NULL;
    }

    IMFAttributes *pAttributes = NULL;
    IMFMediaSource *mediaSource = NULL;

    do {
        // Set the device type to audio.
        HRESULT hr = MFCreateAttributes(&pAttributes, 2);
        hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                                  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
        if (FAILED(hr)) {
            break;
        }

        // Set the endpoint ID.
        hr = pAttributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID,
            (LPCWSTR) m_szAudioEndpointID);
        if (FAILED(hr)) {
            break;
        }

        hr = MFCreateDeviceSource(pAttributes, &mediaSource);
        if (FAILED(hr)) {
            break;
        }

        mediaSource->AddRef();
    } while (false);

    SafeRelease(&pAttributes);

    return mediaSource;
}

void CSJMFCaptureImpl::releaseVideoDeviceInfo() {
    m_videoDevs.clear();
    if (m_videoDevices) {
        for (int i = 0; i < m_videoDevicesCnt; i++) {
            SafeRelease(&m_videoDevices[i]);
        }
        CoTaskMemFree(m_videoDevices);
    }

    m_videoDevicesCnt = 0;
}

IMFMediaType * CSJMFCaptureImpl::getSelectedAudioMediaType(IMFMediaSource * audio_source) {
    if (!audio_source) {
        return NULL;
    }

    IMFPresentationDescriptor *pPD = NULL;
    IMFStreamDescriptor *pSD = NULL;
    IMFMediaTypeHandler *pHandler = NULL;
    IMFMediaType *selMediaType = NULL;

    DWORD selIndex = 0;
    std::string selFmt = "MFAudioFormat_Float";

    std::wstring device_endPoint(m_szAudioEndpointID);
    CSJAudioDeviceInfo deviceInfo = m_audioDeviceInfos[device_endPoint];

    GUID fmtGuid;
    DWORD selSampleRate = 0, selChannels = 0, selBitPerSample = 0;
    CSJAudioCaptureFmtList fmtList = deviceInfo.fmtList;
    auto it = fmtList.begin();
    while (it != fmtList.end()) {
        if (it->fmt_name == selFmt) {
            fmtGuid = it->sub_type;
            selSampleRate = it->sampleRate;
            selChannels = it->channels;
            selBitPerSample = it->bitsPerSample;
        }

        it++;
    }

    bool res = false;
    HRESULT hr = S_OK;
    do {
        hr = audio_source->CreatePresentationDescriptor(&pPD);
        if (FAILED(hr)) {
            break;
        }

        BOOL fSelected;
        hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
        if (FAILED(hr)) {
            break;
        }

        hr = pSD->GetMediaTypeHandler(&pHandler);
        if (FAILED(hr)) {
            break;
        }

        DWORD mediaTypeCnt;
        hr = pHandler->GetMediaTypeCount(&mediaTypeCnt);
        if (FAILED(hr)) {
            break;
        }

        for (int i = 0; i < mediaTypeCnt; i++) {
            IMFMediaType *pType = NULL;
            hr = pHandler->GetMediaTypeByIndex(i, &pType);
            if (FAILED(hr)) {
                continue;
            }

            GUID subtype = { 0 };
            UINT32 channels = 0;
            UINT32 sampleRate = 0;
            UINT32 bitsPerSample = 0;

            hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);
            channels = MFGetAttributeUINT32(pType, MF_MT_AUDIO_NUM_CHANNELS, channels);
            sampleRate = MFGetAttributeUINT32(pType, MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
            bitsPerSample = MFGetAttributeUINT32(pType, MF_MT_AUDIO_BITS_PER_SAMPLE, bitsPerSample);

            if (subtype == fmtGuid && channels == selChannels &&
                sampleRate == selSampleRate && bitsPerSample == selBitPerSample) {
                selMediaType = pType;
                break;
            }

            SafeRelease(&pType);
        }

        if (!selMediaType) {
            break;
        }

        selMediaType->AddRef();
        hr = pHandler->SetCurrentMediaType(selMediaType);
        if (SUCCEEDED(hr)) {
            res = true;
            break;
        }
    } while (false);

    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);

    return selMediaType;
}

void CSJMFCaptureImpl::releaseAudioDeviceInfo() {
    m_audioDevs.clear();
    if (m_audioDevices) {
        for (int i = 0; i < m_audioDevicesCnt; i++) {
            SafeRelease(&m_audioDevices[i]);
        }
        CoTaskMemFree(m_audioDevices);
    }

    m_audioDevicesCnt = 0;
}

void CSJMFCaptureImpl::loadAudioMediaSourceInfos(IMFMediaSource * mediaSource, CSJAudioDeviceInfo & deviceInfo) {
    if (!mediaSource) {
        return;
    }

    IMFPresentationDescriptor *descriptor;
    HRESULT hr = mediaSource->CreatePresentationDescriptor(&descriptor);
    if (FAILED(hr)) {
        return;
    }

    DWORD streamCnt = 0;
    hr = descriptor->GetStreamDescriptorCount(&streamCnt);
    if (FAILED(hr)) {
        return;
    }

    IMFStreamDescriptor *streamDescriptor;
    for (DWORD i = 0; i < streamCnt; i++) {
        BOOL isSelected = FALSE;
        hr = descriptor->GetStreamDescriptorByIndex(i, &isSelected, &streamDescriptor);
        if (FAILED(hr)) {
            continue;
        }

        IMFMediaTypeHandler *typeHandler;
        hr = streamDescriptor->GetMediaTypeHandler(&typeHandler);
        if (FAILED(hr)) {
            continue;
        }

        DWORD mediaTypeCnt;
        hr = typeHandler->GetMediaTypeCount(&mediaTypeCnt);
        if (FAILED(hr)) {
            continue;
        }

        for (DWORD i = 0; i < mediaTypeCnt; i++) {
            IMFMediaType *mediaType;
            hr = typeHandler->GetMediaTypeByIndex(i, &mediaType);
            if (FAILED(hr)) {
                continue;
            }

            BOOL isCompressedFMT = FALSE;
            hr = mediaType->IsCompressedFormat(&isCompressedFMT);
            if (FAILED(hr)) {
                continue;
            }

            if (isCompressedFMT) {
                std::cout << "compressed fmt" << std::endl;
            }

            GUID subtype = { 0 };
            UINT32 channels = 0;
            UINT32 sampleRate = 0;
            UINT32 bitsPerSample = 0;

            hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
            channels = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_NUM_CHANNELS, channels);
            sampleRate = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
            bitsPerSample = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_BITS_PER_SAMPLE, bitsPerSample);

            CSJAudioFmtInfo fmtInfo;
            fmtInfo.sub_type = subtype;
            fmtInfo.fmt_name = SubTypeToString(subtype);
            fmtInfo.channels = channels;
            fmtInfo.sampleRate = sampleRate;
            fmtInfo.bitsPerSample = bitsPerSample;

            deviceInfo.fmtList.push_back(fmtInfo);
        }
    }
}

void CSJMFCaptureImpl::finalize() {
    releaseVideoDeviceInfo();
    releaseAudioDeviceInfo();
}

bool CSJMFCaptureImpl::isSameVideoDevice(int index, WCHAR **symlink) {
    bool res = false;
    if (index >= m_videoDevicesCnt || !m_videoDevices) {
        return res;
    }

    WCHAR *devSymlink;
    UINT32 cchSymlink;
    do {
        HRESULT hr = m_videoDevices[index]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                                               &devSymlink,
                                                               &cchSymlink);
        if (FAILED(hr)) {
            res = false;
            break;
        }

        if (m_szCurCaptureSymlink &&  _wcsicmp(devSymlink, m_szCurCaptureSymlink) == 0) {
            res = true;
            break;
        }
    } while (FALSE);

    // hold the symlink which might be choosen if is needed.
    if (!res && symlink) {
        *symlink = devSymlink;
    } else {
        CoTaskMemFree(devSymlink);
    }
    
    return res;
}

bool CSJMFCaptureImpl::isSameAudioDevice(int index, WCHAR **endpointID) {
    bool res = false;
    if (index >= m_audioDevicesCnt || !m_audioDevices) {
        return res;
    }

    WCHAR *devEndpointID;
    UINT32 cchEndpointID;
    do {
        HRESULT hr = m_audioDevices[index]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID,
                                                               &devEndpointID,
                                                               &cchEndpointID);
        if (FAILED(hr)) {
            res = false;
            break;
        }

        if (m_szCurCaptureSymlink &&  _wcsicmp(devEndpointID, m_szCurCaptureSymlink) == 0) {
            res = true;
            break;
        }
    } while (FALSE);

    // hold the symlink which might be choosen if is needed.
    if (!res && endpointID) {
        *endpointID = devEndpointID;
    } else {
        CoTaskMemFree(devEndpointID);
    }

    return res;
}

bool CSJMFCaptureImpl::createVideoCaptureSource() {
    return false;
}

IMFMediaSource* CSJMFCaptureImpl::createVideoCaptureSourceWithSymlink() {
    IMFAttributes *pAttributes = NULL;
    IMFMediaSource* mediaSource = NULL;

    do {
        HRESULT hr = MFCreateAttributes(&pAttributes, 2);
        if (FAILED(hr)) {
            break;
        }

        hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                                  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        if (FAILED(hr)) {
            break;
        }

        // Set the symbolic link.
        hr = pAttributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                    (LPCWSTR)m_szCurCaptureSymlink);

        if (FAILED(hr)) {
            break;
        }

        hr = MFCreateDeviceSource(pAttributes, &mediaSource);
        if (FAILED(hr)) {
            break;
        }

        mediaSource->AddRef();
    } while (FALSE);

    SafeRelease(&pAttributes);
    return mediaSource;
}

IMFMediaType* CSJMFCaptureImpl::getSelectedVideoMediaType(IMFMediaSource * media_source) {
    if (!media_source) {
        return NULL;
    }

    IMFPresentationDescriptor *pPD = NULL;
    IMFStreamDescriptor *pSD = NULL;
    IMFMediaTypeHandler *pHandler = NULL;
    IMFMediaType *selMediaType = NULL;

    std::string selFmt = "NV12";
    DWORD resolutionIndex = 0;

    std::wstring device_symlink(m_szCurCaptureSymlink);
    CSJVideoDeviceInfo deviceInfo = m_videoDeivceInfos[device_symlink];
    
    GUID fmtGuid;
    DWORD selWidth = 0, selHeight = 0;
    CSJVideoCapureFmtList fmtList = deviceInfo.fmtList;
    auto it = fmtList.begin();
    while (it != fmtList.end()) {
        if (it->fmt_name == selFmt) {
            fmtGuid = it->sub_type;
            selWidth = it->width;
            selHeight = it->height;
        }

        it++;
    }

    bool res = false;
    HRESULT hr = S_OK;
    do {
        hr = media_source->CreatePresentationDescriptor(&pPD);
        if (FAILED(hr)) {
            break;
        }

        BOOL fSelected;
        hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
        if (FAILED(hr)) {
            break;
        }

        hr = pSD->GetMediaTypeHandler(&pHandler);
        if (FAILED(hr)) {
            break;
        }

        DWORD mediaTypeCnt;
        hr = pHandler->GetMediaTypeCount(&mediaTypeCnt);
        if (FAILED(hr)) {
            break;
        }

        for (int i = 0; i < mediaTypeCnt; i++) {
            IMFMediaType *pType = NULL;
            hr = pHandler->GetMediaTypeByIndex(i, &pType);
            if (FAILED(hr)) {
                continue;
            }

            GUID subtype = { 0 };
            UINT32 denominator = 0;
            UINT32 width = 0, height = 0;

            hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);
            hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
            if (subtype == fmtGuid && selWidth == width && selHeight == height) {
                selMediaType = pType;
                SafeRelease(&pType);
                break;
            }

            SafeRelease(&pType);
        }

        if (!selMediaType) {
            break;
        }

        selMediaType->AddRef();
        hr = pHandler->SetCurrentMediaType(selMediaType);
        if (SUCCEEDED(hr)) {
            res = true;
            break;
        }
    } while (false);

    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    
    return selMediaType;
}

bool CSJMFCaptureImpl::createVideoCaptureSink() {
    return false;
}
