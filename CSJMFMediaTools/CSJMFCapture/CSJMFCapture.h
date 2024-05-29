#ifndef __CSJMFCAPTURE_H__
#define __CSJMFCAPTURE_H__

#include "CSJMFMediaToolsDefine.h"

#include <vector>
#include <string>
#include <map>

#include "CSJMediaData.h"

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
 * identify a capture device. For video capture device,
 * the device_identity is symlink, for audio capture device,
 * the device_identity is endpoint.
 */
typedef struct {
    std::wstring device_name;
    std::wstring device_identity; 
} CSJDeviceIdentifier;

/**
*  The capture status
*/
typedef enum {
    CSJMF_CAPTURE_STOP = 0,
    CSJMF_CAPTURE_CAPTURING,
    CSJMF_CAPTURE_PAUSE
} CSJMFCaptureStatus;

typedef struct {
    int width;
    int height;
} CSJVideoResolution;

/**
 * Video capture format param.
 */
typedef struct {
    std::wstring device_name;
    std::wstring device_link;
    std::vector<std::string> formats;
    std::map<std::string, std::vector<CSJVideoResolution>> resolutionMap;
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

    /**
     * @brief Get the device identifier.
     *
     * @param[in] device_type device type.
     */
    virtual std::vector<CSJDeviceIdentifier> getDeviceIdentifiers(CSJMFDeviceType device_type) = 0;

    /**
     * @brief Get the device info.
     *
     * @param[in] identify  it is the symlink.
     */
    virtual CSJVideoDeviceInfo getVideoDeviceInfo(std::wstring identify) = 0;

    /**
     * @brief Get the device info.
     *
     * @param[in] identify  it is endpoint.
     */
    virtual CSJAudioDeviceInfo getAudioDeviceInfo(std::wstring identify) = 0;

};
#endif // __CSJMFCAPTURE_H__