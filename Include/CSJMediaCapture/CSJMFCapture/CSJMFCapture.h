#ifndef __CSJMFCAPTURE_H__
#define __CSJMFCAPTURE_H__

#include "CSJMediaCaptureDefine.h"

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

typedef struct {
    GUID sub_type;
    UINT32 width;
    UINT32 height;
    UINT32 frameRate;
} CSJCameraInfo;

using CSJCemeraInfoList = std::vector<CSJCameraInfo>;

/**
 * This class provides the media capture functionalities
 * based on Media Foundation.
 */
class CSJMEDIACAPTURE_API CSJMFCapture {
public:
    CSJMFCapture() = default;
    virtual ~CSJMFCapture() {};

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

    virtual void stopCapture() = 0;

};
#endif // __CSJMFCAPTURE_H__