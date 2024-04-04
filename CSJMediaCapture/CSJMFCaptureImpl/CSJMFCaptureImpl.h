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
    void resumeCapture() override;
    void stopCapture() override;

    void setDelegate(CSJMFCapture::Delegate *delegate) override;

protected:
    void startCaptureWithSourceReader();

protected:
    /**
    *  Release all the members are relative the media devices.
    */
    void finalize();
    void releaseVideoDeviceInfo();
    
    bool isSameVideoDevice(int index, WCHAR **selectedSymlink = NULL);

    /**
    *  Load all the video devices and their infos.
    */
    void loadVideoDeviceInfos();

    /**
    *  load capture infomations of a cameara.
    *
    *  @param mediaSource the IMFMediaSouce object, represent a camera device.
    *  @param deviceInfo  the video capture device info, fill the fmtList.
    */
    void loadVideoMediaSourceInfos(IMFMediaSource *mediaSource, CSJVideoDeviceInfo& deviceInfo);

    /**
    *  Create the media capture source.
    */
    bool createVideoCaptureSource();

    /**
    *  Create the video capture media source with device symbolic.
    */
    bool createVideoCaptureSourceWithSymlink();

    /**
    *  Set the video capture param. 
    */
    bool setVideoCaptureParam(IMFMediaSource *media_source);

    /**
    *  Check the selected index is same as the current audio device.
    *
    *  @param index, the index of the selected item.
    */
    bool isSameAudioDevice(int index);

    /**
    *  Load all the audio devices and their infos.
    */
    void loadAudioDeviceInfos();

    /**
    *  Create a audio capture media source.
    */
    bool createAudioMediaSource();

    /**
    *  Release all the memebers are relative audio device. 
    */
    void releaseAudioDeviceInfo();

    void loadAudioMediaSourceInfos(IMFMediaSource *mediaSource, CSJVideoDeviceInfo& deviceInfo);

    /**
    *  Create the capture sink, which can get the capture data.
    */
    bool createVideoCaptureSink();

private:
    CSJMFDeviceList m_videoDevs;            // video devices's name list;
    IMFActivate     **m_videoDevices;       // holding the active object for the device.
    UINT32          m_videoDevicesCnt;      // the count of video devices.
    WCHAR           *m_szCurCaptureSymlink; // current video device's symlink, identifier a device.
    IMFMediaSource  *m_videoCapMS;          // video capture media source.

    std::map<std::wstring, CSJVideoDeviceInfo> m_videoDeivceInfos; // <device symlink, device infos>;

    IMFMediaSource *m_audioCapMS;           // audio capture media, as the audio capturing use the default
                                            // audio device, so the following members for audio device are
                                            // not used. I will use these if needed in the future.

    CSJMFDeviceList m_audioDevs;            // audio devices's name list;
    IMFActivate     **m_audioDevices;
    UINT32          m_audioDevicesCnt;
    WCHAR           *m_szAudioEndpointID;   // current audio device's symlink, identifier a device.

    CSJMFCaptureStatus m_status;               // current capture status.
    IMFMediaType       *m_selVideoMediaType;
    IMFMediaType       *m_selAudioMedaiType;


    bool               m_isStop;               // a flag indicates should stop capture or not.

    CSJMFCapture::Delegate *m_delegate;
};

#endif // __CSJMFCAPTUREIMPL_H__