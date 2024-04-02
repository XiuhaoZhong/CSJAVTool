#pragma once

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

protected:
    void updateRenderWindowPos();

    void extractMediaData();

    void extractVideoData();

    void transformMedia();

    void loadMFCapture();

private:

	HDC hdc;
	HGLRC hrc;

    RECT pre_rect_;

	CSJRenderWindow *render_window_;
};

