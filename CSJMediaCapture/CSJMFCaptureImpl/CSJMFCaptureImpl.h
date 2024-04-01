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

    CSJMFDeviceList getVideoCapDevices() override;

    CSJMFDeviceList getAudioCapDevices() override;

protected:
    void getVideoDevices();
    void getAudioDevices();


private:
    CSJMFDeviceList m_videoDevs;        // video devices's name list;
    IMFActivate     **m_videoDevices;   // holding the active object for the device.
    UINT32          m_videoDevicesCnt;  // the count of video devices.

    CSJMFDeviceList m_audioDevs;        // audio devices's name list;
    IMFActivate     **m_audioDevices;
    UINT32          m_audioDevicesCnt;
};

#endif // __CSJMFCAPTUREIMPL_H__