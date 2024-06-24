#ifndef __CSJMFPLAYER_H__
#define __CSJMFPLAYER_H__

#include "CSJMFMediaToolsDefine.h"

#include <memory>
#include <string>

typedef enum {
    CSJMFPLAYER_STATUS_PLAY = 1,
    CSJMFPLAYER_STATUS_PAUSE,
    CSJMFPLAYER_STATUS_STOP
} CSJMFPlayerStatus;

class CSJMFMEDIATOOLS_API CSJMFPlayer {
public:
    CSJMFPlayer() = default;
    virtual ~CSJMFPlayer() {};

    static std::shared_ptr<CSJMFPlayer> generateMFPlayer();

    /**
     * @brief set the file path which will be played.
     *
     * param[in] filePath   the path of the file.
     */
    virtual void setPlayFile(std::wstring& filePath) = 0;

    /**
     * @brief set the handle the window which will used to show video.
     *
     * param[in] hWnd   handle of render window.
     */
    virtual void setHwnd(HWND hWnd) = 0;

    /************************************************************************/
    /* player operations.                                                   */
    /************************************************************************/
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;

    /**
     * @brief get the total duration of the file which will be played.
     *
     * @return duration.
     */
    virtual UINT64 getDuration() = 0;
};

using CSJSpMFPlayer = std::shared_ptr<CSJMFPlayer>;

#endif // __CSJMFPLAYER_H__