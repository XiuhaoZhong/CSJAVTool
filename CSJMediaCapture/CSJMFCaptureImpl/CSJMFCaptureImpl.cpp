#include "CSJMFCaptureImpl.h"

#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>

#include <iostream>

#include "CSJMFCaptureHeader.h"

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

// 在网上发现的从摄像头读取数据的方法，后面试试是否有效 2024/04/02
/*
void ReadDataFromCamera() {
    IMFSourceReader *pReader = NULL;
    IMFMediaSource *pSource = NULL;
    HRESULT hr = S_OK;

    hr = MFCreateSourceReaderFromMediaSource(pSource, NULL, &pReader);
    if (SUCCEEDED(hr)) {
        // 设置输出格式
        hr = pReader->SetCurrentMediaType((DWORD) MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, NULL);
        if (SUCCEEDED(hr)) {
            // 读取帧
            while (SUCCEEDED(hr)) {
                IMFMediaBuffer *pBuffer = NULL;
                DWORD dwStreamIndex, dwStreamFlags;
                LONGLONG llTimeStamp;

                //hr = pReader->ReadSample((DWORD) MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwStreamIndex, &dwStreamFlags, &llTimeStamp, &pBuffer);
                if (SUCCEEDED(hr)) {
                    // 处理帧
                    // ...

                    SafeRelease(&pBuffer);
                }
            }
        }
    }

    SafeRelease(&pReader);
    SafeRelease(&pSource);
}
*/


CSJSharedCapture CSJMFCapture::getMFCapture() {
    return std::make_shared<CSJMFCaptureImpl>();
}

CSJMFCaptureImpl::CSJMFCaptureImpl() {
    m_videoDevicesCnt = 0;
    m_videoDevices = NULL;
    m_szVideoDevSymlink = NULL;

    m_audioDevicesCnt = 0;
    m_audioDevices = NULL;
    m_szAudioDevSymlink = NULL;
}

CSJMFCaptureImpl::~CSJMFCaptureImpl() {
    finalize();

    CoTaskMemFree(m_szVideoDevSymlink);
    m_szVideoDevSymlink = NULL;

    CoTaskMemFree(m_szAudioDevSymlink);
    m_szAudioDevSymlink = NULL;
}

bool CSJMFCaptureImpl::initializeCapture() {
    getVideoDevices();

    if (m_videoDevicesCnt == 0) {
        return false;
    }

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
    CoTaskMemFree(m_szVideoDevSymlink);
    // Save the new device symlink;
    m_szVideoDevSymlink = symlink;

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
    if (isSameAudioDevice(microphone_index)) {
        return;
    }

    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return;
    }

    // 选择麦克风时，如果在采集中，需要切换麦克风
    // TODO: 1、关闭当前麦克风设备
    //       2、重建mediaSession 以及 topology
    //       3、打开新的麦克风设备

}

bool CSJMFCaptureImpl::startCapture() {
    if (!createVideoCaptureSourceWithSymlink()) {
        return false;
    }

    if (!m_videoCapMS) {
        return false;
    }

    IMFPresentationDescriptor *descriptor;
    HRESULT hr = m_videoCapMS->CreatePresentationDescriptor(&descriptor);
    if (FAILED(hr)) {
        return false;
    }

    DWORD streamCnt = 0;
    hr = descriptor->GetStreamDescriptorCount(&streamCnt);
    if (FAILED(hr)) {
        return false;
    }

    IMFStreamDescriptor *streamDescriptor;
    for (DWORD i = 0; i < streamCnt; i++) {
        BOOL isSelected = FALSE;
        hr = descriptor->GetStreamDescriptorByIndex(i, &isSelected, &streamDescriptor);
        if (FAILED(hr)) {
            continue ;
        }

        IMFMediaTypeHandler *typeHandler;
        hr = streamDescriptor->GetMediaTypeHandler(&typeHandler);
        if (FAILED(hr)) {
            continue ;
        }

        DWORD mediaTypeCnt;
        hr = typeHandler->GetMediaTypeCount(&mediaTypeCnt);
        if (FAILED(hr)) {
            continue ;
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
                continue ;
            }

            if (isCompressedFMT) {
                std::cout << "compressed fmt" << std::endl;
            } 

            MFMediaType_Video;
            MFVideoFormat_420O;
            MFVideoFormat_RGB8;
            WCHAR buffer[128];

            MFVideoFormat_AI44;
            
            GUID majorType;
            hr = mediaType->GetMajorType(&majorType);
            if (FAILED(hr)) {
                std::cout << "get major type failed." << std::endl;
            }
            StringFromGUID2(majorType, buffer, sizeof(buffer));
            std::wstring mt(buffer);

            std::string realMT = wstring2string(mt);
            if (realMT.size() > 0) {
                std::cout << realMT << std::endl;
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
            if (FAILED(hr)) {
                std::cout << frameRate << ", " << denominator << std::endl;
            }

            CSJCameraInfo cameraInfo;
            cameraInfo.sub_type = subtype;
            cameraInfo.width = width;
            cameraInfo.height = height;
            cameraInfo.frameRate = frameRate;

            m_cameraInfoList.push_back(cameraInfo);
            // 通过m_cameraInfoList记录下来了一个摄像头支持的采集格式，帧率以及画面尺寸
            // 但是采集格式是GUID，目前还没有转换成字符串，这个问题还没解决
            // 下一步就是设置摄像头采集格式和尺寸，然后得到采集数据 2024/04/02
        }

    }

    hr = m_videoCapMS->Start(descriptor, NULL, NULL);
    return false;
}

void CSJMFCaptureImpl::pauseCapture() {

}

void CSJMFCaptureImpl::stopCapture() {
}

void CSJMFCaptureImpl::getVideoDevices() {
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
        }
    } while (FALSE);

    SafeRelease(&pAttributes);
    pAttributes = NULL;
}

void CSJMFCaptureImpl::getAudioDevices() {
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

        WCHAR *szFriendlyName = NULL;
        UINT32 ccName;
        for (DWORD i = 0; i < m_audioDevicesCnt; i++) {
            // 获取设备的名称，不是唯一，因此不能用来作为判断选择设备的依据.
            hr = m_audioDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                                       &szFriendlyName,
                                                       &ccName);
            if (FAILED(hr)) {
                continue;
            }

            std::wstring devName(szFriendlyName);
            m_audioDevs.push_back(devName);
        }
    } while (FALSE);

    SafeRelease(&pAttributes);
    pAttributes = NULL;
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

        if (m_szVideoDevSymlink &&  _wcsicmp(devSymlink, m_szVideoDevSymlink) == 0) {
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

bool CSJMFCaptureImpl::isSameAudioDevice(int index) {
    return false;
}

bool CSJMFCaptureImpl::createVideoCaptureSource() {
    return false;
}

bool CSJMFCaptureImpl::createVideoCaptureSourceWithSymlink() {
    IMFAttributes *pAttributes = NULL;
    IMFMediaSource *pSource = NULL;

    bool res = false;
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
                                    (LPCWSTR)m_szVideoDevSymlink);

        if (FAILED(hr)) {
            break;
        }

        hr = MFCreateDeviceSource(pAttributes, &m_videoCapMS);
        if (FAILED(hr)) {
            break;
        }

        res = true;
    } while (FALSE);

    SafeRelease(&pAttributes);
    return res;
}

bool CSJMFCaptureImpl::createVideoCaptureSink() {
    return false;
}
