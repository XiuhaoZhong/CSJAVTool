#include "CSJMFCaptureImpl.h"

#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>

#include "CSJMFCaptureHeader.h"

CSJMFCapture* CSJMFCapture::getMFCapture() {
    return new CSJMFCaptureImpl();
}

CSJMFCaptureImpl::CSJMFCaptureImpl() {

}

CSJMFCaptureImpl::~CSJMFCaptureImpl() {

}

CSJMFDeviceList CSJMFCaptureImpl::getVideoCapDevices() {
    return m_videoDevs;
}

CSJMFDeviceList CSJMFCaptureImpl::getAudioCapDevices() {
    return m_audioDevs;
}

void CSJMFCaptureImpl::getVideoDevices() {
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
