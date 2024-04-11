//#include "stdafx.h"

#include "ToolWindow.h"

#include <string>
#include <iostream>

#include "CSJDataManager.h"
#include "CSJRenderWindow.h"
#include "CSJMediaExtracter/CSJMediaExtracter.h"
#include "CSJMediaTransformer/CSJMediaTransformer.h"
#include "CSJMFCapture/CSJMFCapture.h"

#include "CSJAVAudioHandler.h"

const std::wstring ToolWindow::kClassName = L"Basic";

ToolWindow::ToolWindow() {

}

ToolWindow::~ToolWindow() {

}

std::wstring ToolWindow::GetSkinFolder() {
	return L"basic";
}

std::wstring ToolWindow::GetSkinFile() {
	return L"basic.xml";
}

std::wstring ToolWindow::GetWindowClassName() const {
	return kClassName;
}

LONG ToolWindow::GetStyle() {
	//return WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	return WS_OVERLAPPEDWINDOW |  WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
}

void ToolWindow::InitWindow() {
    //extractMediaData();
    //extractVideoData();
    //transformMedia();
    loadMFCapture();
}

LRESULT ToolWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT lRes = 0;

	switch (uMsg) {
	case WM_PAINT:
        RECT cur_rect;
        ::GetWindowRect(m_hWnd, &cur_rect);
        if (!::EqualRect(&cur_rect, &pre_rect_)) {
            pre_rect_ = cur_rect;
        }
        break;

    case WM_MOVE:
        updateRenderWindowPos();
        break;
	
	default:
		break;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void ToolWindow::createRenderWindow() {
	render_window_ = new CSJRenderWindow();

	HWND render_window_hwnd = render_window_->createRenderWindow(GetHWND());
	SetWindowLong(render_window_hwnd, GWL_STYLE, WS_VISIBLE);

	render_window_->initializePos(m_hWnd);
}

LRESULT ToolWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    render_window_->CloseWindow();
    ::DestroyWindow(m_hWnd);
    return 0;
}

LRESULT ToolWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    PostQuitMessage(0);
    return 0;
}

void ToolWindow::updateRenderWindowPos() {
    if (!render_window_) {
        return;
    }

    HWND render_win_hwnd = render_window_->GetHWND();
    if (!render_win_hwnd) {
        return;
    }

    RECT rc;
    memset(&rc, 0, sizeof(RECT));
    int pre_w, pre_h;
    POINT pre_pt;

    if (pre_rect_.right > 0) {
        //POINT pt;
        pre_pt.x = pre_rect_.left;
        pre_pt.y = pre_rect_.top;

        ::ClientToScreen(this->GetHWND(), &pre_pt);
        pre_w = pre_rect_.right - pre_rect_.left;
        pre_h = pre_rect_.bottom - pre_rect_.top;
    }

    RECT cur_area;
    ::GetWindowRect(GetHWND(), &cur_area);

    //if (pre_w == 0 || pre_h == 0) {
        SetWindowPos(render_win_hwnd, GetHWND(), cur_area.left + 50, cur_area.top + 50, 400, 300, SWP_NOZORDER);
        //return;
    //}
}

void ToolWindow::extractMediaData() {
    std::string str1 = "partyAnimals.mp4";
    std::string str2 = "partyAnimals.aac";
    CSJSharedExtracter mediaExtracter = CSJMediaExtracter::getExtracter(str1, str2, CSJ_EXTRACTDATA_AUDIO);
    if (!mediaExtracter) {
        std::cout << "media extracter created successfully" << std::endl;
        return ;
    }

    bool ret = mediaExtracter->extractData();
    if (!ret) {
        std::cout << "extracting audio failed." << std::endl;
    } else {
        std::cout << "extracting audio successful." << std::endl;
    }
}

void ToolWindow::extractVideoData() {
    std::string str1 = "partyAnimals.mp4";
    std::string str2 = "partyAnimals.h264";
    CSJSharedExtracter mediaExtracter = CSJMediaExtracter::getExtracter(str1, str2, CSJ_EXTRACTDATA_VIDEO);
    if (!mediaExtracter) {
        std::cout << "media extracter created successfully" << std::endl;
        return;
    }

    bool ret = mediaExtracter->extractData();
    if (!ret) {
        std::cout << "extracting audio failed." << std::endl;
    } else {
        std::cout << "extracting audio successful." << std::endl;
    }
}

void ToolWindow::transformMedia() {
    std::string str1 = "partyAnimals.mp4";
    std::string str2 = "partyAnimals.flv";

    CSJSharedTransfomer mediaTransformer = CSJMediaTransformer::getTransformerWithFileName(str1, str2);
    if (!mediaTransformer) {
        return;
    }

    bool ret = mediaTransformer->transformMedia();
    if (!ret) {
        std::cout << "Transform media file failed!" << std::endl;
    }
}

static std::shared_ptr<CSJAVAudioHandler> audioHandler;
void ToolWindow::loadMFCapture() {
    audioHandler = std::make_shared<CSJAVAudioHandler>();
    
    //audioHandler->init();

    //audioHandler->startCapture();   
    audioHandler->testPlayer();
}