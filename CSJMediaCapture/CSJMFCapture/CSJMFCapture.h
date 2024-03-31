#ifndef __CSJMFCAPTURE_H__
#define __CSJMFCAPTURE_H__

#include "CSJMediaCaptureDefines.h"

#include <vector>
#include <string>
#include <map>

using CSJMFDeviceList = std::vector<std::string>;
using CSJMFDeviceMap = std::map<std::wstring, std::wstring>;

/**
 * The device type.
 */
typedef enum {
    CSJMF_DEVICE_VIDEO = 0,
    CSJMF_DEVICE_AUDIO
} CSJMFDeviceType;

/**
 * This class provides the media capture functionalities
 * based on Media Foundation.
 */
class CSJMEDIACAPTURE_API CSJMFCapture {
public:
    CSJMFCapture() = default;
    virtual ~CSJMFCapture() {};

    static CSJMFCapture* getMFCapture();

    virtual CSJMFDeviceList getCaptureDevices(CSJMFDeviceType deviceType) = 0;

};
#endif // __CSJMFCAPTURE_H__