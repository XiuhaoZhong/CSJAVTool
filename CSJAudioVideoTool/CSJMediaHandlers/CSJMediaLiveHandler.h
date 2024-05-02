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

    /************************************************************************/
    /* Implements CSJMediaHandlerBase                                       */
    /************************************************************************/
    void stop() override;

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

private:
    CSJSharedCapture  m_pCapture;
    bool              m_bCaptureInit;
    int               m_selCapFormatIndex;
    int               m_selCapResolutionIndex;

    CSJSharedEncoder  m_pEncoder;
    bool              m_bEncoderInit;

    HWND              m_pRenderWindow;
    bool              m_bRendererInit;
};

#endif // __CSJMEDIALIVEHANDLER_H__