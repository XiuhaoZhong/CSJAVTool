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

const int renderWindowLeftDelta = 164;
const int renderWindowTopDelta = 50;
const int renderWindowWidth = 550;
const int renderWindowHeight = 450;

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
	return WS_OVERLAPPEDWINDOW |  WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
}

void ToolWindow::InitWindow() {
    //extractMediaData();
    //extractVideoData();
    //transformMedia();
    //loadMFCapture();

    m_pMainFrameView = dynamic_cast<ui::Box *>((FindControl(L"MainFrameBox")));
    m_pCaptureBtn = dynamic_cast<ui::Button *>((FindControl(L"CaptureBtn")));
    m_pPlayerBtn = dynamic_cast<ui::Button *>((FindControl(L"PlayerBtn")));
    m_pTransCodeBtn = dynamic_cast<ui::Button *>((FindControl(L"TransCodeBtn")));

    if (m_pCaptureBtn) {
        m_pCaptureBtn->AttachClick(nbase::Bind(&ToolWindow::onBtnClicked, this, std::placeholders::_1));
    }

    m_pCaptureBox = dynamic_cast<ui::HBox *>((FindControl(L"CaptureBox")));
    m_pCaptureBackBtn = dynamic_cast<ui::Button *>((FindControl(L"CaptureReturnBtn")));
    if (m_pCaptureBackBtn) {
        m_pCaptureBackBtn->AttachClick(nbase::Bind(&ToolWindow::onBtnClicked, this, std::placeholders::_1));
    }

    createRenderWindow();
    showRenderWindow(false);
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
	render_window_ = new CSJRenderWindow(renderWindowLeftDelta, renderWindowTopDelta, renderWindowWidth, renderWindowHeight);

	HWND render_window_hwnd = render_window_->createRenderWindow(GetHWND());
}

LRESULT ToolWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (render_window_) {
        showRenderWindow(false);
        render_window_->CloseWindow();
    }
    
    ::DestroyWindow(m_hWnd);
    return 0;
}

LRESULT ToolWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    PostQuitMessage(0);
    return 0;
}

bool ToolWindow::onBtnClicked(ui::EventArgs *args) {
    if (args->pSender == m_pCaptureBtn) {
        m_pMainFrameView->SetVisible(false);
        showCaptureBox(true);
    } else if (args->pSender == m_pCaptureBackBtn) {
        m_pMainFrameView->SetVisible(true);
        showCaptureBox(false);
    }
    return true;
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

    SetWindowPos(render_win_hwnd, GetHWND(),
                 cur_area.left + renderWindowLeftDelta,
                 cur_area.top + renderWindowTopDelta,
                 renderWindowWidth,
                 renderWindowHeight,
                 SWP_NOZORDER);
}

void ToolWindow::showRenderWindow(bool bShow) {
    if (!render_window_) {
        return ;
    }

    int nCmd = bShow ? SW_SHOW : SW_HIDE;
    ::ShowWindow(render_window_->GetHWND(), nCmd);
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
    
    audioHandler->init();

    audioHandler->startCapture();   
    //audioHandler->testPlayer();
}

void ToolWindow::showCaptureBox(bool show) {
    m_pCaptureBox->SetVisible(show);

    showRenderWindow(show);

    if (show) {
        loadVideoDeviceList();
    }
}

void ToolWindow::loadVideoDeviceList() {
    if (!m_pVideoDevcieCombo) {
        m_pVideoDevcieCombo = dynamic_cast<ui::Combo *>((FindControl(L"CameraDeviceCombo")));
        m_pVideoDevcieCombo->SetPopupTop(false);
    }

    m_pVideoDevcieCombo->RemoveAll();
    for (int i = 0; i < 3; i++) {
        std::wstring number = std::to_wstring(i);
        std::wstring text = L"Device" + number;
        ui::ListContainerElement *element = createListElement(text, i, 100, 20);
        if (element) {
            m_pVideoDevcieCombo->Add(element);
        }
    }
    m_pVideoDevcieCombo->SelectItem(0);

    if (!m_pVideoFmtCombo) {
        m_pVideoFmtCombo = dynamic_cast<ui::Combo *>((FindControl(L"CameraFmtCombo")));
        m_pVideoFmtCombo->SetPopupTop(false);
    }

    m_pVideoFmtCombo->RemoveAll();
    std::vector<std::wstring> fmts = { L"NV12", L"MJPG", L"YV12" };
    for (int i = 0; i < fmts.size(); i++) {
        ui::ListContainerElement *element = createListElement(fmts[i], i, 100, 20);
        if (element) {
            m_pVideoFmtCombo->Add(element);
        }
    }
    m_pVideoFmtCombo->SelectItem(0);

    
    if (!m_pVideoResolutionCombo) {
        m_pVideoResolutionCombo = dynamic_cast<ui::Combo *>((FindControl(L"CameraResolutionCombo")));
        m_pVideoResolutionCombo->SetPopupTop(false);
    }

    m_pVideoResolutionCombo->RemoveAll();
    std::vector<std::wstring> resolutions = { L"1280x720", L"640x480", L"480x320" };
    for (int i = 0; i < fmts.size(); i++) {
        ui::ListContainerElement *element = createListElement(resolutions[i], i, 100, 20);
        if (element) {
            m_pVideoResolutionCombo->Add(element);
        }
    }

    m_pVideoResolutionCombo->SelectItem(0);
}

ui::ListContainerElement * ToolWindow::createListElement(std::wstring & text, 
                                                         int index,
                                                         int fixedWidth,
                                                         int fixedHeight,
                                                         std::wstring bkcolor,
                                                         std::wstring selectedcolor) {
    ui::ListContainerElement *listElement = new ui::ListContainerElement();
    if (!listElement) {
        return nullptr;
    }
    listElement->SetText(text);
    listElement->SetIndex(index);
    listElement->SetFixedWidth(fixedWidth > 0 ? fixedWidth : DUI_LENGTH_AUTO);
    listElement->SetFixedHeight(fixedHeight > 0 ? fixedHeight: DUI_LENGTH_AUTO);
    listElement->SetBkColor(bkcolor);
    listElement->SetSelectedStateColor(ui::kControlStateNormal, selectedcolor);

    return listElement;
}
