#include "CSJMFCaptureImpl.h"

#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mmdeviceapi.h>
#include <wmcodecdsp.h>
#include <mftransform.h>
#include <mfreadwrite.h>

#include <iostream>
#include <fstream>

#include "CSJMFMediaHeader.h"
#include "CSJStringTool/CSJStringTool.h"
#include "CSJMediaData/CSJMediaData.h"

CSJSharedCapture CSJMFCapture::getMFCapture() {
    return std::make_shared<CSJMFCaptureImpl>();
}

CSJMFCaptureImpl::CSJMFCaptureImpl() {
    m_videoDevicesCnt = 0;
    m_videoDevices = NULL;

    m_audioDevicesCnt = 0;
    m_audioDevices = NULL;

    m_delegate = nullptr;
    m_isStop = true;
}

CSJMFCaptureImpl::~CSJMFCaptureImpl() {
    finalize();
}

bool CSJMFCaptureImpl::initializeCapture() {
    loadVideoDeviceInfos();

    loadAudioDeviceInfos();

    return true;
}

void CSJMFCaptureImpl::setOutputAsRGB24(bool outputRGB24) {
    m_outputRGB24 = true;
}

void CSJMFCaptureImpl::selectedCamera(int camera_index, int format_index, int resolution_index) {
    WCHAR *symlink;
    
    if (isSameVideoDevice(camera_index, &symlink) && (
        camera_index == m_selVideoDevIndex &&
        format_index == m_selVideoFmtIndex &&
        resolution_index == m_selVideoResolutionIndex)) {
        return ;
    }

    m_selVideoDevIndex = camera_index;
    m_selVideoFmtIndex = format_index;
    m_selVideoResolutionIndex = resolution_index;

    // At this stage, if you want to change the video capture parameter, you must
    // select the new parameter from the UI, and then stop capture, and start catpure.
    // I will improve to change capture parameters without restarting capture by hand
    // in the future.
    return;

    // Save the new device symlink;
    m_szCurCaptureSymlink = symlink;

    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return ;
    }

    // 选择摄像头时，如果在采集中，需要切换摄像头
    // TODO: 1、关闭当前视频采集设备
    //       2、重建mediaSession 以及 topology
    //       3、打开新的视频设备
    stop();

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

    m_szAudioEndpointID = endpointID;

    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return;
    }

    // 选择麦克风时，如果在采集中，需要切换麦克风
    // TODO: 1、关闭当前麦克风设备
    //       2、重建mediaSession 以及 topology
    //       3、打开新的麦克风设备

}

bool CSJMFCaptureImpl::start() {    
    m_videoCapThread = std::thread(&CSJMFCaptureImpl::startVideoCapWithSourceReader, this);
    //m_audioCapThread = std::thread(&CSJMFCaptureImpl::startAudioCapWithSourceReader, this);
    return true;
}

void CSJMFCaptureImpl::pause() {
    if (m_status != CSJMF_CAPTURE_CAPTURING) {
        return;
    }

    m_status = CSJMF_CAPTURE_PAUSE;
}

void CSJMFCaptureImpl::resume() {
    if (m_status != CSJMF_CAPTURE_PAUSE) {
        return ;
    }

    m_status = CSJMF_CAPTURE_CAPTURING;
}

void CSJMFCaptureImpl::stop() {
    if (m_status == CSJMF_CAPTURE_STOP) {
        return ;
    }

    m_isStop = true;
}

void CSJMFCaptureImpl::setDelegate(CSJMFCapture::Delegate * delegate) {
    m_delegate = delegate;
}

std::vector<CSJDeviceIdentifier> CSJMFCaptureImpl::getDeviceIdentifiers(CSJMFDeviceType device_type) {
    if (device_type == CSJMF_DEVICE_VIDEO) {
        return m_videoDeviceIdentifiers;
    } else {
        return m_audioDeviceIdentifiers;
    }
}

std::vector<CSJVideoDeviceInfo> CSJMFCaptureImpl::getVideoDeviceInfo() {
    return m_videoDeviceInfos;
}

std::vector<CSJAudioDeviceInfo>  CSJMFCaptureImpl::getAudioDeviceInfo() {
    return m_audioDeviceInfos;
}

bool CSJMFCaptureImpl::isStarted() {
    return !m_isStop;
}

void CSJMFCaptureImpl::startVideoCapWithSourceReader() {
    CComPtr<IMFMediaSource> mediaSource = createVideoCaptureSourceWithSymlink();
    if (!mediaSource) {
        return ;
    }

    CComPtr<IMFMediaType> mediaType = getSelectedVideoMediaType(mediaSource);
    if (!mediaType) {
        return ;
    }

    CComPtr<IMFSourceReader> pReader = NULL;
    HRESULT hr = S_OK;

    CComPtr<IMFMediaSink> pSink = NULL;

    // create media source to read video data.
    hr = MFCreateSourceReaderFromMediaSource(mediaSource, NULL, &pReader);
    if (SUCCEEDED(hr)) {
        // Setting the mediaType of output format.
        hr = pReader->SetCurrentMediaType((DWORD) MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, mediaType);
        if (SUCCEEDED(hr)) {
            m_isStop = false;
            m_status = CSJMF_CAPTURE_CAPTURING;

            // 读取帧
            IMFSample *pBuffer = NULL;
            DWORD dwStreamIndex, dwStreamFlags;
            LONGLONG llTimeStamp;
            FILE *outYuvFile = fopen("capture.yuv", "wb+");
            while (true) {
                if (m_isStop) {
                    break;
                }

                hr = pReader->ReadSample((DWORD) MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwStreamIndex, &dwStreamFlags, &llTimeStamp, &pBuffer);
                if (SUCCEEDED(hr)) {
                    // At the first time returning of the ReadSample, the pBuffer might null.
                    if (pBuffer == NULL) {
                        continue;
                    }

                    DWORD sampleLen = 0;
                    pBuffer->GetBufferCount(&sampleLen);
                    pBuffer->SetSampleTime(llTimeStamp);

                    // Note: you must take the following operation on the buf
                    // 1.Using CComptr on constraints the buf
                    // 2.Using raw pointer on the buf, and SafeRelease the buf at the end of the circle.
                    // or the ReadSample won't return in future.
                    CComPtr<IMFMediaBuffer> buf = NULL;
                    if (m_outputRGB24) {
                        buf = convertToRGB24(pBuffer, mediaType);
                        if (!buf) {
                            continue;
                        }
                    } else {
                        hr = pBuffer->GetBufferByIndex(0, &buf);
                    }

                    if (m_delegate) {
                        m_delegate->onVideoArrive(buf, llTimeStamp);
                    }
                    
                    SafeRelease(&pBuffer);

                }
            }
        }
    }

    if (m_outputRGB24) {
        m_transformer = nullptr;
    }

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

    // 为麦克风采集创建相应的SourceReader.
    hr = MFCreateSourceReaderFromMediaSource(mediaSource, NULL, &pReader);
    if (SUCCEEDED(hr)) {
        // 设置输出格式
        hr = pReader->SetCurrentMediaType((DWORD) MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, mediaType);
        if (SUCCEEDED(hr)) {
            m_isStop = false;
            m_status = CSJMF_CAPTURE_CAPTURING;

            LONGLONG time1 = 0;
            // 读取帧
            std::fstream outFile;
            outFile.open("capture.txt", std::ios::out);
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

                    time1 = llTimeStamp;
            
                    IMFMediaBuffer *buf;
                    pBuffer->GetBufferByIndex(0, &buf);
                    if (buf) {

                        BYTE *buffer = NULL;
                        DWORD maxLen;
                        hr = buf->Lock(&buffer, &maxLen, &sampleLen);
                        if (FAILED(hr)) {
                            std::cout << "lock sample failed." << std::endl;
                        }

                        DWORD curLength;
                        hr = buf->GetCurrentLength(&curLength);
                        if (FAILED(hr)) {
                            std::cout << "" << std::endl;
                        }

                        /*CSJMFAudioData *audioData = new CSJMFAudioData(pBuffer);
                        if (m_delegate) {
                            m_delegate->onAudioDataArrive(audioData);
                        }
                        delete audioData;*/
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

    CComPtr<IMFAttributes> pAttributes = NULL;
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

            WCHAR *devSymlink;
            UINT32 cchSymlink;
            hr = m_videoDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                                       &devSymlink,
                                                       &cchSymlink);

            m_videoDeviceIdentifiers.push_back({ szFriendlyName, devSymlink });

            CSJVideoDeviceInfo deviceInfo;
            deviceInfo.deviceIdentity = { szFriendlyName, devSymlink };

            CComPtr<IMFMediaSource> pSource = NULL;
            hr = m_videoDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));
            if (FAILED(hr)) {
                break;
            }

            loadVideoMediaSourceInfos(pSource, deviceInfo);
            m_videoDeviceInfos.emplace_back(deviceInfo);
        }
    } while (FALSE);
}

void CSJMFCaptureImpl::loadVideoMediaSourceInfos(CComPtr<IMFMediaSource> mediaSource, 
                                                 CSJVideoDeviceInfo &deviceInfo) {
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
            CComPtr<IMFMediaType> mediaType = NULL;
            hr = typeHandler->GetMediaTypeByIndex(i, &mediaType);
            if (FAILED(hr)) {
                continue;
            }

            BOOL isCompressedFMT = FALSE;
            hr = mediaType->IsCompressedFormat(&isCompressedFMT);
            if (FAILED(hr)) {
                continue;
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

            std::wstring fmt_name = SubTypeToString(subtype);
            std::vector<std::wstring> &fmts = deviceInfo.formats;
            bool fmtExist = false;
            for (auto format : fmts) {
                if (format.compare(fmt_name) == 0) {
                    fmtExist = true;
                    break;
                }
            }

            if (!fmtExist) {
                fmts.push_back(fmt_name);
            }

            CSJVideoFmtInfo fmtInfo = { frameRate, {(int)width, (int)height} };
            deviceInfo.fmtInfo[fmt_name].push_back(fmtInfo);
            m_videoSubtypes.push_back(mediaType);
        }
    }
}

void CSJMFCaptureImpl::loadAudioDeviceInfos() {
    releaseAudioDeviceInfo();

    CComPtr<IMFAttributes> pAttributes = NULL;

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

            CComPtr<IMFMediaSource> pSource = NULL;
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

            m_audioDeviceIdentifiers.push_back({ szFriendlyName, devEndPoint });
            
            std::wstring endPointID(devEndPoint);
            CSJAudioDeviceInfo deviceInfo;
            deviceInfo.deviceIndentity = { szFriendlyName, endPointID };
            loadAudioMediaSourceInfos(pSource, deviceInfo);
            m_audioDeviceInfos.emplace_back(deviceInfo);
        }
    } while (FALSE);
}

IMFMediaSource * CSJMFCaptureImpl::createAudioCaptureSourceWithEndPoint() {
    if (m_szAudioEndpointID.empty()) {
        return NULL;
    }

     CComPtr<IMFAttributes> pAttributes = NULL;
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
            (LPCWSTR) m_szAudioEndpointID.c_str());
        if (FAILED(hr)) {
            break;
        }

        hr = MFCreateDeviceSource(pAttributes, &mediaSource);
        if (FAILED(hr)) {
            break;
        }

        mediaSource->AddRef();
    } while (false);

    return mediaSource;
}

void CSJMFCaptureImpl::releaseVideoDeviceInfo() {
    m_videoDeviceInfos.clear();
    m_videoSubtypes.clear();

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

    CComPtr<IMFMediaType> selMediaType = NULL;

    int selDevIndex = 0;
    auto selDevInfo = m_audioDeviceInfos[selDevIndex];
    CSJAudioFmtInfo selFmtInfo = selDevInfo.fmtList[0];
    auto subtypeIt = m_audioSubtypes.begin();
    while (subtypeIt != m_audioSubtypes.end()) {
        GUID subtype = { 0 };
        UINT32 channels = 0;
        UINT32 sampleRate = 0;
        UINT32 bitsPerSample = 0;

        (*subtypeIt)->GetGUID(MF_MT_SUBTYPE, &subtype);
        channels = MFGetAttributeUINT32((*subtypeIt), MF_MT_AUDIO_NUM_CHANNELS, channels);
        sampleRate = MFGetAttributeUINT32((*subtypeIt), MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
        bitsPerSample = MFGetAttributeUINT32((*subtypeIt), MF_MT_AUDIO_BITS_PER_SAMPLE, bitsPerSample);

        std::wstring fmtName = SubTypeToString(subtype);
        if (fmtName.compare(selFmtInfo.fmt_name) == 0 &&
            selFmtInfo.bitsPerSample == bitsPerSample &&
            selFmtInfo.channels == channels &&
            selFmtInfo.sampleRate == sampleRate) {
            
            selMediaType = (*subtypeIt);
            break;
        }
    }

    return selMediaType;
}

void CSJMFCaptureImpl::releaseAudioDeviceInfo() {
    m_audioDeviceInfos.clear();
    m_audioSubtypes.clear();

    if (m_audioDevices) {
        for (int i = 0; i < m_audioDevicesCnt; i++) {
            SafeRelease(&m_audioDevices[i]);
        }
        CoTaskMemFree(m_audioDevices);
    }

    m_audioDevicesCnt = 0;
}

void CSJMFCaptureImpl::loadAudioMediaSourceInfos(CComPtr<IMFMediaSource> mediaSource, CSJAudioDeviceInfo & deviceInfo) {
    if (!mediaSource) {
        return;
    }

    CComPtr<IMFPresentationDescriptor> descriptor;
    HRESULT hr = mediaSource->CreatePresentationDescriptor(&descriptor);
    if (FAILED(hr)) {
        return;
    }

    DWORD streamCnt = 0;
    hr = descriptor->GetStreamDescriptorCount(&streamCnt);
    if (FAILED(hr)) {
        return;
    }

    CComPtr<IMFStreamDescriptor> streamDescriptor;
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
            CComPtr<IMFMediaType> mediaType = NULL;
            hr = typeHandler->GetMediaTypeByIndex(i, &mediaType);
            if (FAILED(hr)) {
                continue;
            }

            BOOL isCompressedFMT = FALSE;
            hr = mediaType->IsCompressedFormat(&isCompressedFMT);
            if (FAILED(hr)) {
                continue;
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
            m_audioSubtypes.push_back(mediaType);
        }
    }
}

CComPtr<IMFTransform> CSJMFCaptureImpl::createTransformWithType() {
    CComPtr<IMFTransform> transformer = nullptr;

    CComPtr<IUnknown> colorConvTransformUnk = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CColorConvertDMO, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IUnknown, (void **)&colorConvTransformUnk);
    if (FAILED(hr)) {
        return nullptr;
    }

    hr = colorConvTransformUnk->QueryInterface(IID_PPV_ARGS(&transformer));
    if (FAILED(hr)) {
        return nullptr;
    }

    return transformer;
}

CComPtr<IMFMediaBuffer> CSJMFCaptureImpl::convertToRGB24(CComPtr<IMFSample> inputSample, CComPtr<IMFMediaType> inputType) {
    CComPtr<IMFMediaBuffer> buf = nullptr;

    if (!m_transformer) {
        m_transformer = createTransformWithType();
    }

    if (!m_transformer) {
        return buf;
    }

    HRESULT hr = m_transformer->SetInputType(0, inputType, 0);
    if (FAILED(hr)) {
        return buf;
    }

    CComPtr<IMFMediaType> outputType = nullptr;
    hr = MFCreateMediaType(&outputType);
    if (FAILED(hr)) {
        return buf;
    }

    hr = inputType->CopyAllItems(outputType);
    if (FAILED(hr)) {
        return buf;
    }

    hr = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    hr = outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24);
    if (FAILED(hr)) {
        return buf;
    }

    CComPtr<IMFMediaType> avaMediaType = nullptr;
    hr = m_transformer->GetOutputAvailableType(0, 0, &avaMediaType);
    if (FAILED(hr)) {
        return buf;
    }

    GUID subType;
    hr = avaMediaType->GetGUID(MF_MT_SUBTYPE, &subType);
    if (FAILED(hr)) {
        return buf;
    }

    hr = m_transformer->SetOutputType(0, outputType, 0);
    if (FAILED(hr)) {
        return buf;
    }

    DWORD mftStatus = 0;
    hr = m_transformer->GetInputStatus(0, &mftStatus);
    if (FAILED(hr)) {
        return buf;
    }

    hr = m_transformer->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
    hr = m_transformer->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
    hr = m_transformer->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);

    hr = m_transformer->ProcessInput(0, inputSample, NULL);
    if (FAILED(hr)) {
        return buf;
    }

    MFT_OUTPUT_STREAM_INFO si = { 0 };
    mftStatus = 0;
    CComPtr<IMFSample> pOutputSample = nullptr;
    CComPtr<IMFMediaBuffer> pOutputBuffer = nullptr;
    MFT_OUTPUT_DATA_BUFFER mftOutBuffer;

    hr = m_transformer->GetOutputStreamInfo(0, &si);
    hr = MFCreateSample(&pOutputSample);
    hr = MFCreateMemoryBuffer(si.cbSize, &pOutputBuffer);
    hr = pOutputSample->AddBuffer(pOutputBuffer);

    mftOutBuffer.dwStreamID = 0;
    mftOutBuffer.dwStatus = 0;
    mftOutBuffer.pEvents = NULL;
    mftOutBuffer.pSample = pOutputSample;

    hr = m_transformer->ProcessOutput(0, 1, &mftOutBuffer, &mftStatus);
    if (FAILED(hr)) {
        return buf;
    }

    hr = pOutputSample->ConvertToContiguousBuffer(&buf);
    if (FAILED(hr)) {
        return buf;
    }

    return buf;
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

        std::wstring tmpLink(devSymlink);
        if (!m_szCurCaptureSymlink.empty() && m_szCurCaptureSymlink.compare(tmpLink) == 0) {
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

    WCHAR *devEndpointID = NULL;
    UINT32 cchEndpointID = 0;
    do {
        HRESULT hr = m_audioDevices[index]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID,
                                                               &devEndpointID,
                                                               &cchEndpointID);
        if (FAILED(hr)) {
            res = false;
            break;
        }

        std::wstring tmpEndpoint(devEndpointID);
        if (!m_szCurCaptureSymlink.empty() &&  m_szCurCaptureSymlink.compare(tmpEndpoint) == 0) {
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

CComPtr<IMFMediaSource> CSJMFCaptureImpl::createVideoCaptureSourceWithSymlink() {
    CComPtr<IMFAttributes> pAttributes = NULL;
    CComPtr<IMFMediaSource> mediaSource = NULL;

    m_szCurCaptureSymlink = m_videoDeviceInfos[0].deviceIdentity.device_identity;

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
                                    (LPCWSTR)m_szCurCaptureSymlink.c_str());

        if (FAILED(hr)) {
            break;
        }

        hr = MFCreateDeviceSource(pAttributes, &mediaSource);
        if (FAILED(hr)) {
            break;
        }

    } while (FALSE);

    return mediaSource;
}

CComPtr<IMFMediaType> CSJMFCaptureImpl::getSelectedVideoMediaType(IMFMediaSource * media_source) {
    if (!media_source) {
        return NULL;
    }

    CComPtr<IMFMediaType> selMediaType = NULL;
    int deviceIndex = m_selVideoDevIndex, fmtIndex = m_selVideoFmtIndex, resolutionIndex = m_selVideoResolutionIndex;

    // selected capture parameters.
    CSJVideoDeviceInfo selDev = m_videoDeviceInfos[deviceIndex];
    std::wstring selFormat = selDev.formats[fmtIndex];
    CSJVideoResolution selResolution = selDev.fmtInfo[selFormat][resolutionIndex].resolution;

    DWORD selWidth = 0, selHeight = 0;
    auto it = m_videoSubtypes.begin();
    while (it != m_videoSubtypes.end()) {
        GUID subtype = { 0 };
        UINT32 frameRate = 0;
        UINT32 denominator = 0;
        UINT32 width = 0, height = 0;
        UINT32 frameRateMin = 0, frameRateMax = 0;

        (*it)->GetGUID(MF_MT_SUBTYPE, &subtype);
        MFGetAttributeSize((*it), MF_MT_FRAME_SIZE, &width, &height);
        std::wstring subtypeStr = SubTypeToString(subtype);

        if (subtypeStr.compare(selFormat) == 0 && selResolution.width == width && selResolution.height == height) {
            selMediaType = *it;
            break;
        }
    }

    return selMediaType;
}

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
