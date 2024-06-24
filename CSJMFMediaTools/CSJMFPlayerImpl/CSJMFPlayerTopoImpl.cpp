#include "CSJMFPlayerTopoImpl.h"

#include <iostream>

std::shared_ptr<CSJMFPlayerTopoImpl> CSJMFPlayerTopoImpl::spPlayer = nullptr;

CSJSpMFPlayer CSJMFPlayer::generateMFPlayer() {
    return CSJMFPlayerTopoImpl::getInstance();
}

CSJMFPlayerTopoImpl::CSJMFPlayerTopoImpl() {

}

CSJMFPlayerTopoImpl::~CSJMFPlayerTopoImpl() {

}

std::shared_ptr<CSJMFPlayerTopoImpl> CSJMFPlayerTopoImpl::getInstance() {
    if (!spPlayer) {
        spPlayer = std::make_shared<CSJMFPlayerTopoImpl>();
    }
    return spPlayer;
}

void CSJMFPlayerTopoImpl::setPlayFile(std::wstring & filePath) {
    m_filePath = filePath;
}

void CSJMFPlayerTopoImpl::setHwnd(HWND hWnd) {
    m_pHwnd = hWnd;
}

void CSJMFPlayerTopoImpl::start() {
    if (m_status == CSJMFPLAYER_STATUS_PLAY) {
        return;
    }
    m_playThread = std::thread(&CSJMFPlayerTopoImpl::startInternal, this);
}

void CSJMFPlayerTopoImpl::pause() {
    if (m_status != CSJMFPLAYER_STATUS_PLAY) {
        return ;
    }
    m_pMediaSession->Pause();
}

void CSJMFPlayerTopoImpl::resume() {
    if (m_status != CSJMFPLAYER_STATUS_PAUSE) {
        return;
    }

    m_pMediaSession->Start(&GUID_NULL, NULL);
}

void CSJMFPlayerTopoImpl::stop() {
    if (m_status != CSJMFPLAYER_STATUS_STOP) {
        m_pMediaSession->Stop();
    }
}

UINT64 CSJMFPlayerTopoImpl::getDuration() {

    IMFMediaSource *mediaSource = NULL;
    IMFPresentationDescriptor *pPd;
    HRESULT hr = mediaSource->CreatePresentationDescriptor(&pPd);
    if (FAILED(hr)) {
        return 0;
    }

    UINT64 duration = 0;
    hr = pPd->GetUINT64(MF_PD_DURATION, &duration);
    if (FAILED(hr)) {
        return 0;
    }

    return duration;
}

bool CSJMFPlayerTopoImpl::init() {
    CComPtr<IMFSourceResolver> pSourceResolver = nullptr;
    HRESULT hr = MFCreateSourceResolver(&pSourceResolver);
    if (FAILED(hr)) {
        // failed to create source resolver.
        return false;
    }

    MF_OBJECT_TYPE ObjectType = MF_OBJECT_TYPE::MF_OBJECT_INVALID;
    CComPtr<IUnknown> uSource = nullptr;
    hr = pSourceResolver->CreateObjectFromURL(m_filePath.c_str(),
                                              MF_RESOLUTION_MEDIASOURCE,
                                              NULL,
                                              &ObjectType,
                                              &uSource);
    if (FAILED(hr)) {
        // failed to create mf object.
        return false;
    }

    CComPtr<IMFMediaSession> mediaSession;
    hr = MFCreateMediaSession(NULL, &mediaSession);
    if (FAILED(hr)) {
        // failed to create media session.
        return false;
    }
    m_pMediaSession = mediaSession;

    CComPtr<IMFTopology> mediaTopology;
    hr = MFCreateTopology(&mediaTopology);
    if (FAILED(hr)) {
        // failed to create media topology.
        return false;
    }
    m_pMediaTopo = mediaTopology;

    CComPtr<IMFMediaSource> pSource;
    hr = uSource->QueryInterface(IID_PPV_ARGS(&pSource));
    if (FAILED(hr)) {
        // failed to query interface media source.
        return false;
    }

    CComPtr<IMFPresentationDescriptor> pSourcePD;
    hr = pSource->CreatePresentationDescriptor(&pSourcePD);
    if (FAILED(hr)) {
        // failed to create presentation description.
        return false;
    }

    UINT64 totalDuration = 0;
    hr = pSourcePD->GetUINT64(MF_PD_DURATION, &totalDuration);
    if (FAILED(hr)) {
        // failed to get totalDuration.
    }

    UINT64 seconds = totalDuration * 100 / (1000000000);
    if (seconds == 100) {
    
    }

    DWORD sourceStreamCount;
    hr = pSourcePD->GetStreamDescriptorCount(&sourceStreamCount);
    if (FAILED(hr)) {
        // failed to get stream count.
        return false;
    }

    for (DWORD i = 0; i < sourceStreamCount; i++) {
        BOOL fSelected = FALSE;
        GUID guidMajorType;

        CComPtr<IMFStreamDescriptor> pSourceSD = nullptr;
        hr = pSourcePD->GetStreamDescriptorByIndex(i, &fSelected, &pSourceSD);
        if (FAILED(hr)) {
            // failed to get stream descriptor.
            continue;
        }

        CComPtr<IMFMediaTypeHandler> pHandler = nullptr;
        hr = pSourceSD->GetMediaTypeHandler(&pHandler);
        if (FAILED(hr)) {
            // failed to get media type handler.
            continue;
        }

        hr = pHandler->GetMajorType(&guidMajorType);
        if (FAILED(hr)) {
            // failed to get major type.
            continue;
        }

        if (guidMajorType == MFMediaType_Audio && fSelected) {
            CComPtr<IMFActivate> pActivate = nullptr;
            hr = MFCreateAudioRendererActivate(&pActivate);
            if (FAILED(hr)) {
                // failed to create audio renderer activate.
                continue;
            }

            CComPtr<IMFTopologyNode> pSourceNode = nullptr;
            hr = addSourceNode(mediaTopology, pSource, pSourcePD, pSourceSD, pSourceNode);
            if (FAILED(hr)) {
                // failed to add audio source node.
                continue;
            }

            CComPtr<IMFTopologyNode> pSinkNode = nullptr;
            hr = addOutputNode(mediaTopology, pActivate, 0, pSinkNode);
            hr = pSourceNode->ConnectOutput(0, pSinkNode, 0);
            if (FAILED(hr)) {
                // audio source node connects to output node failed.
                continue;
            }

            parseMediaInfo(pSource, true);

        } else if (guidMajorType == MFMediaType_Video && fSelected) {
            CComPtr<IMFActivate> pActivate = nullptr;
            hr = MFCreateVideoRendererActivate(m_pHwnd, &pActivate);
            if (FAILED(hr)) {
                // failed to create audio renderer activate.
                continue;
            }

            CComPtr<IMFTopologyNode> pSourceNode = nullptr;
            hr = addSourceNode(mediaTopology, pSource, pSourcePD, pSourceSD, pSourceNode);
            if (FAILED(hr)) {
                // failed to add audio source node.
                continue;
            }

            CComPtr<IMFTopologyNode> pSinkNode = nullptr;
            hr = addOutputNode(mediaTopology, pActivate, 0, pSinkNode);
            hr = pSourceNode->ConnectOutput(0, pSinkNode, 0);
            if (FAILED(hr)) {
                // audio source node connects to output node failed.
                continue;
            }
            
            parseMediaInfo(pSource, true);

        } else {
            hr = pSourcePD->DeselectStream(i);
            if (FAILED(hr)) {
                // failed to dselectec stream.
            }
        }
    }
       
    return true;
}

void CSJMFPlayerTopoImpl::startInternal() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        // failed to initialize com.
        return ;
    }

    hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        // failed to startup MediaFoundation.
        return ;
    }

    if (init()) {
        CComPtr<IMFMediaEvent> pEvent = nullptr;
        MediaEventType meType = MEUnknown;
        HRESULT hrState = S_OK;

         hr = m_pMediaSession->SetTopology(0, m_pMediaTopo);
        if (FAILED(hr)) {
            // failed to set topology to media session.
            return;
        }

        bool exitWithStop = false;
        while (meType != MESessionClosed) {
            if (exitWithStop) {
                hr = m_pMediaSession->Close();
                if (FAILED(hr)) {
                    // failed close after media seseion stop.
                    break;
                }
            }

            hr = m_pMediaSession->GetEvent(0, &pEvent);
            if (FAILED(hr)) {
                break;
            }

            hr = pEvent->GetType(&meType);
            if (FAILED(hr)) {
                break;
            }

            hr = pEvent->GetStatus(&hrState);
            if (FAILED(hr)) {
                break;
            }

            switch (meType) {
            case MESessionTopologySet:
                PROPVARIANT var;
                hr = m_pMediaSession->Start(&GUID_NULL, &var);
                if (FAILED(hr)) {
                    // failed to start media session.
                    return;
                }
                break;
            case MESessionStarted:
                wprintf_s(L"Player started...\n");
                m_status = CSJMFPLAYER_STATUS_PLAY;
                break;
            case MESessionPaused:
                wprintf_s(L"Player pause...\n");
                m_status = CSJMFPLAYER_STATUS_PAUSE;
                break;
            case MESessionStopped:
                wprintf_s(L"Player stop...\n");
                m_status = CSJMFPLAYER_STATUS_STOP;
                exitWithStop = true;
                break;
            case MESessionEnded:
                wprintf_s(L"Player end...\n");
                hr = m_pMediaSession->Close();
                break;
            case MESessionClosed:
                wprintf_s(L"Output file created.\n");
                break;
            case MESessionNotifyPresentationTime:
                pEvent->GetValue(&var);
                MF_EVENT_START_PRESENTATION_TIME_AT_OUTPUT;
                break;
            }

            pEvent = nullptr;
        }

        m_pMediaSession->Shutdown();
        m_pMediaSession = nullptr;
    }

    MFShutdown();
    CoUninitialize();
}

HRESULT CSJMFPlayerTopoImpl::addSourceNode(CComPtr<IMFTopology> pTopology, 
                                           CComPtr<IMFMediaSource> pSource, 
                                           CComPtr<IMFPresentationDescriptor> pPD, 
                                           CComPtr<IMFStreamDescriptor> pSD, 
                                           CComPtr<IMFTopologyNode>& ppNode) {
    CComPtr<IMFTopologyNode> pNode = nullptr;

    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);
    if (FAILED(hr)) {
        // failed to create topology node,
        return hr;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
    if (FAILED(hr)) {
        // failed to set source on topology node.
        return hr;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
    if (FAILED(hr)) {
        // failed set presentation on topology node.
        return hr;
    }

    hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
    if (FAILED(hr)) {
        // failed set stream descriptor on topology node.
        return hr;
    }

    hr = pTopology->AddNode(pNode);
    if (FAILED(hr)) {
        // failed add node to topology.
        return hr;
    }

    ppNode = pNode;
    return hr;
}

HRESULT CSJMFPlayerTopoImpl::addOutputNode(CComPtr<IMFTopology> pTopology, 
                                           CComPtr<IMFActivate> pActivate, 
                                           DWORD dwId, 
                                           CComPtr<IMFTopologyNode>& ppNode) {
    CComPtr<IMFTopologyNode> pNode = nullptr;

    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);
    if (FAILED(hr)) {
        // failed to create topology node.
        return hr;
    }

    hr = pNode->SetObject(pActivate);
    if (FAILED(hr)) {
        // failed to set sink on topology node.
        return hr;
    }

    hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId);
    if (FAILED(hr)) {
        // failed to set stream id on topology node.
        return hr;
    }

    hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
    if (FAILED(hr)) {
        // failed to set no shutdown on topology node.
        return hr;
    }

    hr = pTopology->AddNode(pNode);
    if (FAILED(hr)) {
        // failed to add node to topology.
        return hr;
    }

    ppNode = pNode;
    return hr;
}

void CSJMFPlayerTopoImpl::parseMediaInfo(CComPtr<IMFMediaSource> mediaSource, bool isVideoOrAudio) {
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

            if (isVideoOrAudio) {
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


                //GUID subtype = { 0 };
                UINT32 channels = 0;
                UINT32 sampleRate = 0;
                UINT32 bitsPerSample = 0;
                UINT32 payload = 0;

                hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
                channels = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_NUM_CHANNELS, channels);
                sampleRate = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
                //payload = MFGetAttributeUINT32(mediaType, MF_MT_AAC_PAYLOAD_TYPE, payload);
                bitsPerSample = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_BITS_PER_SAMPLE, bitsPerSample);
                hr = mediaType->GetUINT32(MF_MT_AAC_PAYLOAD_TYPE, &payload);
                if (FAILED(hr)) {
                    // get MF_MT_AAC_PAYLOAD_TYPE failed。
                }
                if (sampleRate > 0) {
                    std::cout << "" << std::endl;
                }

            } else {
                GUID subtype = { 0 };
                UINT32 channels = 0;
                UINT32 sampleRate = 0;
                UINT32 floatSampleRate = 0;
                UINT32 bitsPerSample = 0;

                hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
                channels = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_NUM_CHANNELS, channels);
                sampleRate = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
                floatSampleRate = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND, floatSampleRate);
                //MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND
                bitsPerSample = MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_BITS_PER_SAMPLE, bitsPerSample);
                if (sampleRate > 0) {
                    std::cout << "" << std::endl;
                }
            }
        }
    }

}
