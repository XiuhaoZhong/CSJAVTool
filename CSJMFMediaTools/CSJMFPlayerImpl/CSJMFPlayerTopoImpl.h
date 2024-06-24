#ifndef __CSJMFPLAYERTOPOIMPL_H__
#define __CSJMFPLAYERTOPOIMPL_H__

#include "CSJMFPlayer/CSJMFPlayer.h"

#include <atlbase.h>

#include <thread>

class CSJMFPlayerTopoImpl : public CSJMFPlayer {
public:
    CSJMFPlayerTopoImpl();
    ~CSJMFPlayerTopoImpl();

    static std::shared_ptr<CSJMFPlayerTopoImpl> getInstance();

    void setPlayFile(std::wstring& filePath) override;

    void setHwnd(HWND hWnd) override;

    void start() override;
    void pause() override;
    void resume() override;
    void stop() override;

    UINT64 getDuration() override;

protected:
    bool init();

    void startInternal();

    HRESULT addSourceNode(CComPtr<IMFTopology> pTopology,
                          CComPtr<IMFMediaSource> pSource,
                          CComPtr<IMFPresentationDescriptor> pPD,
                          CComPtr<IMFStreamDescriptor> pSD,
                          CComPtr<IMFTopologyNode> &ppNode);

    HRESULT addOutputNode(CComPtr<IMFTopology> pTopology,
                          CComPtr<IMFActivate> pActivate,
                          DWORD dwId,
                          CComPtr<IMFTopologyNode> &ppNode);

    /**
     * @brief get media detail info with mediasouce.
     *
     * param[in] mediaSource    the media source of the media data.
     * param[in] isVideoOrAudio the type of data, true indicates media is video, otherwise, audio.
     */
    void parseMediaInfo(CComPtr<IMFMediaSource> mediaSource, bool isVideoOrAudio);

private:
    HWND              m_pHwnd;
    std::wstring      m_filePath;
    std::thread       m_playThread;
    CSJMFPlayerStatus m_status;

    CComPtr<IMFMediaSession> m_pMediaSession;
    CComPtr<IMFTopology>     m_pMediaTopo;

    static std::shared_ptr<CSJMFPlayerTopoImpl> spPlayer;
};



#endif // __CSJMFPLAYERTOPOIMPL_H__