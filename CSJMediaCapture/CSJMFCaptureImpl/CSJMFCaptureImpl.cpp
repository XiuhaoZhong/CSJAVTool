#include "CSJMFCaptureImpl.h"

#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>

CSJMFCapture* CSJMFCapture::getMFCapture() {
    return new CSJMFCaptureImpl();
}

CSJMFCaptureImpl::CSJMFCaptureImpl() {

}

CSJMFCaptureImpl::~CSJMFCaptureImpl() {

}

CSJMFDeviceList CSJMFCaptureImpl::getCaptureDevices(CSJMFDeviceType deviceType) {
    IMFMediaSource *pSource = NULL;
    IMFAttributes *pAttributes = NULL;
    IMFActivate **ppDevices = NULL;

    HRESULT hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr)) {
        return CSJMFDeviceList();
    }

    hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (FAILED(hr)) {
        //SafeRelease(&pAttributes);
    }

    UINT32 count;
    hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
    if (FAILED(hr)) {
    
    }

    if (count == 0) {
    
    }
    
    WCHAR *szFriendlyName = NULL;
    UINT32 ccName;
    for (DWORD i = 0; i < count; i++) {
        // 获取设备的名称，不是唯一，因此不能用来作为判断选择设备的依据.
        hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, 
                                              &szFriendlyName, 
                                              &ccName);
        if (FAILED(hr)) {
            continue ;
        }

        // 获取一个视频采集的符号链接，可以唯一标识一个视频采集设备;
        // 也可以通过此链接创建一个采集数据源.
        WCHAR *szDeviceSymbLink;
        UINT32 symLinkLength;
        hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                              &szDeviceSymbLink,
                                              &symLinkLength);

        // TODO: WCHAR 需要转换成string，考虑添加一个常用工具的dll;
        // TODO: 目前获取捕获设备已经OK，接下来是要学习怎么给设备设置
        //       具体的捕获参数

        std::wstring deviceSymb(szDeviceSymbLink);
        std::wstring deviceName(szFriendlyName);

        m_videoDevMap.insert({deviceSymb, deviceName});
    }

    return CSJMFDeviceList();
}