#ifndef __CSJMFCAPTURE_H__
#define __CSJMFCAPTURE_H__

#include "CSJMFMediaToolsDefine.h"

#include <vector>
#include <string>
#include <map>

using CSJMFDeviceList = std::vector<std::wstring>;
using CSJMFDeviceMap = std::map<std::wstring, std::wstring>;

class CSJMFCapture;
using CSJSharedCapture = std::shared_ptr<CSJMFCapture>;

/**
 * The device type.
 */
typedef enum {
    CSJMF_DEVICE_VIDEO = 0,
    CSJMF_DEVICE_AUDIO
} CSJMFDeviceType;

/**
*  The capture status
*/
typedef enum {
    CSJMF_CAPTURE_STOP = 0,
    CSJMF_CAPTURE_CAPTURING,
    CSJMF_CAPTURE_PAUSE
} CSJMFCaptureStatus;

/*
*  Video capture format param.
*/
typedef struct {
    GUID        sub_type;
    UINT32      width;
    UINT32      height;
    UINT32      frameRate;
    std::string fmt_name;
} CSJVideoFmtInfo;

using CSJVideoCapureFmtList = std::vector<CSJVideoFmtInfo>;

/**
*  Video capture device params.
*/
typedef struct {
    std::wstring            device_name;
    std::wstring            device_symlink;
    CSJVideoCapureFmtList   fmtList;
} CSJVideoDeviceInfo;

/**
*  Audio capture format params.
*/
typedef struct {
    GUID        sub_type;       // audio capture format guid.
    UINT32      channels;       
    UINT32      sampleRate;
    UINT32      bitsPerSample;  // audio capture format name.
    std::string fmt_name;
} CSJAudioFmtInfo;

using CSJAudioCaptureFmtList = std::vector<CSJAudioFmtInfo>;

/**
*  Audio capture device params.
*/
typedef struct {
    std::wstring            device_name;
    std::wstring            device_endPintID;
    CSJAudioCaptureFmtList  fmtList;
} CSJAudioDeviceInfo;

/**
 * This class provides the media capture functionalities
 * based on Media Foundation.
 */
class CSJMFMEDIATOOLS_API CSJMFCapture {
public:
    CSJMFCapture() = default;
    virtual ~CSJMFCapture() {};

    /**
    * This class provides a series interfaces for receiving capture data and 
    * status.
    */
    class Delegate {
    public:
        Delegate() = default;
        ~Delegate() = default;

        /**
        *  The video data arrived.
        */
        virtual void onVideDataArrive() = 0;

        /**
        *  The audio data arrived.
        */
        virtual void onAudioDataArrive(CSJMFAudioData *audioData) = 0;

        /**
        *  Occurs errors during the capturing.
        */
        virtual void onErrorOccurs() = 0;

        /**
        *  Capturing status changed.
        */
        virtual void onStatusChanged() = 0;
    };

    static CSJSharedCapture getMFCapture();

    /*
    *  Initialize the capture;
    */
    virtual bool initializeCapture() = 0;

    /**
    *  Get video capture devices name list.
    */
    virtual CSJMFDeviceList getVideoCapDevices() = 0;

    /**
    *  Get audio capture devices name list.
    */
    virtual CSJMFDeviceList getAudioCapDevices() = 0;

    /**
     *  Selected a camera to capture.
     *
     *  @param camera_index the index of the device.
     */
    virtual void selectedCamera(int camera_index) = 0;

    /**
    *  Selected a microphone to capture.
    *  
    *  @param microphone_index the index of the device.
    */
    virtual void selectedMicrophone(int microphone_index) = 0;

    /************************************************************************/
    /*  Capture operations.                                                 */
    /************************************************************************/
    virtual bool startCapture() = 0;

    virtual void pauseCapture() = 0;

    virtual void resumeCapture() = 0;

    virtual void stopCapture() = 0;

    /**
    *  Set the delegate which could deliver capture data, error, status to external.
    */
    virtual void setDelegate(CSJMFCapture::Delegate *delegate) = 0;

};
#endif // __CSJMFCAPTURE_H__