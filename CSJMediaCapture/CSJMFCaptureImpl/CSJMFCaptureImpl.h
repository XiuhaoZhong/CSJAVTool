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

    bool initializeCapture() override;

    CSJMFDeviceList getVideoCapDevices() override;

    CSJMFDeviceList getAudioCapDevices() override;

    void selectedCamera(int camera_index) override;

    void selectedMicrophone(int microphone_index) override;

    bool startCapture() override;
    void pauseCapture() override;
    void stopCapture() override;

protected:
    void getVideoDevices();
    void getAudioDevices();

    void releaseVideoDeviceInfo();
    void releaseAudioDeviceInfo();

    void finalize();

    bool isSameVideoDevice(int index, WCHAR **selectedSymlink = NULL);

    bool isSameAudioDevice(int index);

    /**
    *  Create the media capture source.
    */
    bool createVideoCaptureSource();

    /*
    *  Create the video capture media source with device symbolic.
    */
    bool createVideoCaptureSourceWithSymlink();

    /**
    *  Create the capture sink, which can get the capture data.
    */
    bool createVideoCaptureSink();

    /**
    *  Create audio capture source; 
    */

private:
    CSJMFDeviceList m_videoDevs;            // video devices's name list;
    IMFActivate     **m_videoDevices;       // holding the active object for the device.
    UINT32          m_videoDevicesCnt;      // the count of video devices.
    WCHAR           *m_szVideoDevSymlink;   // current video device's symlink, identifier a device.

    CSJMFDeviceList m_audioDevs;            // audio devices's name list;
    IMFActivate     **m_audioDevices;
    UINT32          m_audioDevicesCnt;
    WCHAR           *m_szAudioDevSymlink;   // current audio device's symlink, identifier a device.

    CSJMFCaptureStatus m_status;            // current capture status.

    IMFMediaSource *m_videoCapMS;           // video capture media source.
    CSJCemeraInfoList m_cameraInfoList;
};

#endif // __CSJMFCAPTUREIMPL_H__