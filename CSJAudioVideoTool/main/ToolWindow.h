#pragma once

#include "base/base.h"
#include "duilib/UIlib.h"

#include "duilib/Utils/WinImplBase.h"

class CSJRenderWindow;

class CSJMediaLiveFrame;
class CSJMediaPlayerFrame;
class CSJTestFrame;

class ToolWindow : public ui::WindowImplBase {
public:
	ToolWindow();
	~ToolWindow();

	/**
	 * 
	 */

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	virtual LONG GetStyle() override;

	virtual void InitWindow() override;

    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void createRenderWindow();

	static const std::wstring kClassName;

    bool onBtnClicked(ui::EventArgs *args);

protected:
    void updateRenderWindowPos();

    void showRenderWindow(bool bShow);

    void extractMediaData();

    void extractVideoData();

    void transformMedia();

    void loadMFCapture();

    void showCaptureBox(bool show);

    void showPlayerFrame(bool show);

    void showTestFrame(bool show);

    void testMFPlayer();

private:
	HDC hdc;
	HGLRC hrc;

    RECT pre_rect_;

    ui::Box *m_pMainUI = nullptr;

    /**
     * Main frame view.
     */
    ui::Box     *m_pMainFrameView = nullptr;
    ui::Button  *m_pCaptureBtn    = nullptr;
    ui::Button  *m_pPlayerBtn     = nullptr;
    ui::Button  *m_pTransCodeBtn  = nullptr;
    ui::Button  *m_pTestFrameBtn  = nullptr;

    CSJMediaLiveFrame   *m_pMediaLiveFrame = nullptr;
    CSJMediaPlayerFrame *m_pMediaPlayerFrame = nullptr;

    CSJTestFrame        *m_pTestFrame = nullptr;

	CSJRenderWindow     *render_window_;
};

