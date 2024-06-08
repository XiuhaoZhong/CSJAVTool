#ifndef __CSJMEDIALIVEHANDLER_H__
#define __CSJMEDIALIVEHANDLER_H__

#include "CSJMediaHandlerBase.h"

#include "CSJMFCapture/CSJMFCapture.h"
#include "CSJMediaEncoder/CSJMediaEncoder.h"

class CSJMediaLiveHandler : public CSJMediaHandlerBase,
                            public CSJMFCapture::Delegate,
                            public CSJMediaEncoder::Delegate {
public:
    CSJMediaLiveHandler();
    ~CSJMediaLiveHandler();

    /**
     * @brief Init live handler
     */
    bool initLiveHandler();

    /**
     * @brief Selects the capture device with index.
     * 
     * After selected a device, the format and reslution will be reset to default index
     * at 0.
     *
     * @param[in] selIndex  the index of the device which is selected.
     */
    void selectCaptureDevice(int selIndex);

    /**
     * @brief selects the capture format with index.
     *
     * @param[in] selIndex  the index of the selected format.
     */
    void selectCaptureFormat(int selIndex);

    /**
     * @brief selects the capture resolution with index.
     *
     * @param[in] selIndex  the index of the selected resolution.
     */
    void selectCaptureResolution(int selIndex);

    /**
     * @brief set the handle of render window.
     * 
     * @param[in] hwnd  the handle of render window.
     */
    void setRenderWindow(HWND hwnd);

    /**
     * @brief starts capture.
     *
     * @return success return true, or return false.
     */
    bool startCapture();

    /**
     * @biref get current capture status.
     *
     * @return return true if it's capturing, or return false.
     */
    bool isCapturing();

    /************************************************************************/
    /* Implements CSJMediaHandlerBase                                       */
    /************************************************************************/
    void stopHandler() override;

    /************************************************************************/
    /* Implements CSJMFCapture::Delegate.                                   */
    /************************************************************************/
    void onVideDataArrive() override;

    void onAudioDataArrive(CSJMFAudioData *audioData) override;

    void onErrorOccurs() override;

    void onStatusChanged() override;

    /************************************************************************/
    /* Implements CSJMediaEncoder::Delegate.                                */
    /************************************************************************/
    void onEncoderDataArrived(uint8_t *data, int dataSize) override;
    
    /**
     * @brief get all the audio capture device names.
     *
     * @param[out] save the audio capture device names.
     */
    void getAudioCapDevNames(std::vector<std::wstring>& names);

    /**
     * @brief get all the video capture device names.
     *
     * @param[out] save the video capture device names.
     */
    void getVideoCapDevNames(std::vector<std::wstring>& names);

    /**
     * @berif Get all the formats that the selected video capture device supports.
     *
     * @param[in]  devIndex     the index of selected video capture device.
     * @param[out] formats      the formats of selected video capture device supports.
     */
    void getFormatsWithDevIndex(int devIndex, std::vector<std::wstring>& formats);

    /**
     * @brief Get all resolutions under the device Index and format index.
     * 
     * @param[in]  devIndex     the index of selected video capture device.
     * @param[in]  fmtIndex     the format index of selected video capture device.
     * @param[out] resolutions  the resolutions.
     */
    void getResolutionsWithDevIndexAndFmtIndex(int devIndex, int fmtIndex, std::vector<std::wstring>& resolutions);

protected:
    /**
     * @berif Init media capture.
     */
    bool initCapture();

    /**
     * @brief Init media encoder.
     */
    bool initEncoder();

    /**
     * @brief Init renderer.
     */

    bool initRender();

    /**
     * @brief reload the capture device infos.
     */
    void reloadCaptureInfos();

private:
    CSJSharedCapture  m_pCapture;
    bool              m_bCaptureInit;
    int               m_selCapDeviceIndex;
    int               m_selCapFormatIndex;
    int               m_selCapResolutionIndex;

    CSJSharedEncoder  m_pEncoder;
    bool              m_bEncoderInit;

    HWND              m_pRenderWindow;
    bool              m_bRendererInit;

    std::vector<CSJVideoDeviceInfo> m_videoCapInfos;
    std::vector<CSJAudioDeviceInfo> m_audioCapInfos;

};


using CSJSpMediaLiveHandler = std::shared_ptr<CSJMediaLiveHandler>;

#endif // __CSJMEDIALIVEHANDLER_H__