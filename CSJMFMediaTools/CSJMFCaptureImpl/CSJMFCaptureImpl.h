#ifndef __CSJMFCAPTUREIMPL_H__
#define __CSJMFCAPTUREIMPL_H__

#include "CSJMFCapture.h"

#include <mutex>
#include <thread>
#include <atlcomcli.h>

#include <Audioclient.h>

/**
 * This class inplements CSJMFCapture interfaces.
 */
class CSJMFCaptureImpl : public CSJMFCapture {
public:
    CSJMFCaptureImpl();
    ~CSJMFCaptureImpl();

    bool initializeCapture() override;

    void selectedCamera(int camera_index) override;

    void selectedMicrophone(int microphone_index) override;

    bool start() override;
    void pause() override;
    void resume() override;
    void stop() override;

    void setDelegate(CSJMFCapture::Delegate *delegate) override;

    virtual std::vector<CSJDeviceIdentifier> getDeviceIdentifiers(CSJMFDeviceType device_type) override;

    virtual std::vector<CSJVideoDeviceInfo> getVideoDeviceInfo() override;

    virtual std::vector<CSJAudioDeviceInfo>  getAudioDeviceInfo() override;

    virtual bool isStarted() override;

protected:
    /**
    *  Starts Video capture with seleted video meida type.
    */
    void startVideoCapWithSourceReader();

    /**
    *  Start Audio capture with selected audio media type.
    */
    void startAudioCapWithSourceReader();

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
    void loadVideoMediaSourceInfos(CComPtr<IMFMediaSource> mediaSource,  CSJVideoDeviceInfo &deviceInfo);

    /**
    *  Create the media capture source.
    */
    bool createVideoCaptureSource();

    /**
    *  Create the video capture media source with device symbolic.
    */
    CComPtr<IMFMediaSource> createVideoCaptureSourceWithSymlink();

    /**
    *  Get selected video capture type.
    *  
    *  @param media_source, the selected media source which will capture video.
    *
    *  @return IMFMediaType, the media type which the video capturing wants.
    */
    CComPtr<IMFMediaType> getSelectedVideoMediaType(IMFMediaSource *media_source);

    /**
    *  Check the selected index is same as the current audio device.
    *
    *  @param index, the index of the selected item.
    *  @param endpointID[out], return the selected endpointID if Needed. 
    */
    bool isSameAudioDevice(int index, WCHAR **endpointID = NULL);

    /**
    *  Load all the audio devices and their infos.
    */
    void loadAudioDeviceInfos();

    /**
    *  Create the audio media source with the selected endpoint;
    */
    IMFMediaSource* createAudioCaptureSourceWithEndPoint();

    /**
    *  Get selected audio media type which will be used to indicate the capture's info.
    *
    *  @param audio_source, the audio media source which will capture audio.
    *
    *  @return IMFMediaType, the media type which the audio caputring wants.
    */
    IMFMediaType* getSelectedAudioMediaType(IMFMediaSource *audio_source);

    /**
    *  Release all the memebers are relative audio device. 
    */
    void releaseAudioDeviceInfo();

    void loadAudioMediaSourceInfos(CComPtr<IMFMediaSource> mediaSource, CSJAudioDeviceInfo& deviceInfo);

private:
    IMFActivate         **m_videoDevices;       // holding the active object for the device.
    UINT32              m_videoDevicesCnt;      // the count of video devices.
    std::wstring        m_szCurCaptureSymlink;  // current video device's symlink, identifier a device.
    IMFMediaType        *m_selVideoMediaType;

    IMFActivate         **m_audioDevices;
    UINT32              m_audioDevicesCnt;
    std::wstring        m_szAudioEndpointID;    // current audio device's symlink, identifier a device.
    IMFMediaType        *m_selAudioMedaiType;

    std::thread         m_videoCapThread;
    std::thread         m_audioCapThread;

    std::vector<CSJDeviceIdentifier>             m_videoDeviceIdentifiers;
    std::vector<CComPtr<IMFMediaType>>           m_videoSubtypes;
    std::vector<CSJDeviceIdentifier>             m_audioDeviceIdentifiers;
    std::vector<CComPtr<IMFMediaType>>           m_audioSubtypes;

    std::vector<CSJVideoDeviceInfo>              m_videoDeviceInfos;
    std::vector<CSJAudioDeviceInfo>              m_audioDeviceInfos;

    CSJMFCaptureStatus  m_status;               // current capture status.
    bool                m_isStop;               // a flag indicates should stop capture or not.

    CSJMFCapture::Delegate *m_delegate;
};

#endif // __CSJMFCAPTUREIMPL_H__