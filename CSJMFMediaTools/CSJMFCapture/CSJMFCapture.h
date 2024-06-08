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
 * video format info, includes resolution and frame rate.
 */
typedef struct {
    int                frameRate;
    CSJVideoResolution resolution;
} CSJVideoFmtInfo;

/**
 * Video capture format param.
 */
typedef struct {
    CSJDeviceIdentifier deviceIdentity;
    std::vector<std::wstring> formats;
    std::map<std::wstring, std::vector<CSJVideoFmtInfo>> fmtInfo;
} CSJVideoDeviceInfo;

/**
*  Audio capture format params.
*/
typedef struct {
    GUID         sub_type;       // audio capture format guid.
    UINT32       channels;       
    UINT32       sampleRate;
    UINT32       bitsPerSample;  // audio capture format name.
    std::wstring fmt_name;
} CSJAudioFmtInfo;

using CSJAudioCaptureFmtList = std::vector<CSJAudioFmtInfo>;

/**
*  Audio capture device params.
*/
typedef struct {
    CSJDeviceIdentifier     deviceIndentity;
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
    virtual bool start() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    virtual void stop() = 0;

        /**
     * @brief wheather the device is capturing.
     *
     * @return  ture if it's captruing.
     */
    virtual bool isStarted() = 0;

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
     * @brief Get video device infos.
     */
    virtual std::vector<CSJVideoDeviceInfo> getVideoDeviceInfo() = 0;

    /**
     * @brief Get the device info.
     *
     * @param[in] identify  it is endpoint.
     */
    virtual std::vector<CSJAudioDeviceInfo>  getAudioDeviceInfo() = 0;

};
#endif // __CSJMFCAPTURE_H__