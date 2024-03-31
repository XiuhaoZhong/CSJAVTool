#ifndef __CSJMFCAPTUREIMPL_H__
#define __CSJMFCAPTUREIMPL_H__

#include "CSJMFCapture.h"

/**
 * This class inplements CSJMFCapture interfaces.
 */
class CSJMFCaptureImpl : public CSJMFCapture {
public:
    CSJMFCaptureImpl();
    ~CSJMFCaptureImpl();

    CSJMFDeviceList getCaptureDevices(CSJMFDeviceType deviceType) override;

private:
    CSJMFDeviceMap m_videoDevMap;
    CSJMFDeviceMap m_audioDevMap;
};

#endif // __CSJMFCAPTUREIMPL_H__