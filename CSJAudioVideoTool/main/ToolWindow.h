#pragma once

#include "base/base.h"
#include "duilib/UIlib.h"

#include "duilib/Utils/WinImplBase.h"

class CSJRenderWindow;

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

    void loadVideoDeviceList();

    /**
     * @brief create list element with the params.
     *
     * @param[in] text          the text the element show.
     * @param[in] index         the index of current element.
     * @param[in] fixedWidth    the fixed width of the element, if users want to 
                                auto estimating by system, pass 0.
     * @param[in] fixedHeight   the fixed height of the element, pass 0 as same 
                                with fixedWidth.

     * @param[in] bkcolor       the background color of current element, default is white.
     * @param[in] selectedcolor the background of selected status, default is blue.
     *
     * @return    if success return a pointer points the list element, or return nullptr.
     */
    ui::ListContainerElement* createListElement(std::wstring &text, 
                                                int index,
                                                int fixedWidth,
                                                int fixedHeight,
                                                std::wstring bkcolor = L"white",
                                                std::wstring selecedcolor = L"blue");

private:
	HDC hdc;
	HGLRC hrc;

    RECT pre_rect_;

    /**
     * Main frame view.
     */
    ui::Box     *m_pMainFrameView = nullptr;
    ui::Button  *m_pCaptureBtn    = nullptr;
    ui::Button  *m_pPlayerBtn     = nullptr;
    ui::Button  *m_pTransCodeBtn  = nullptr;

    /**
     * Capture box.
     */
    ui::HBox    *m_pCaptureBox           = nullptr;
    ui::Button  *m_pCaptureBackBtn       = nullptr;
    ui::Combo   *m_pVideoDevcieCombo     = nullptr;
    ui::Combo   *m_pVideoFmtCombo        = nullptr;
    ui::Combo   *m_pVideoResolutionCombo = nullptr;

	CSJRenderWindow *render_window_;
};

