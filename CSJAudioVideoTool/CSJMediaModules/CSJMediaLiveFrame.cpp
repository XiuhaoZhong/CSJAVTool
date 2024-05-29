#include "CSJMediaLiveFrame.h"

#include <string>

CSJMediaLiveFrame::CSJMediaLiveFrame()
    : CSJUIModuleBase(L"", nullptr, nullptr) {
}

CSJMediaLiveFrame::CSJMediaLiveFrame(ui::STRINGorID & xml, ui::Window * manager, ui::Box * parent)
    : CSJUIModuleBase(xml, manager, parent) {

}

CSJMediaLiveFrame::~CSJMediaLiveFrame() {

}

void CSJMediaLiveFrame::initUI() {
    m_pCaptureBox = dynamic_cast<ui::HBox *>((getManager()->FindControl(L"CaptureBox")));
    m_pCaptureBackBtn = dynamic_cast<ui::Button *>((getManager()->FindControl(L"CaptureReturnBtn")));
    if (m_pCaptureBackBtn) {
        m_pCaptureBackBtn->AttachClick(nbase::Bind(&CSJMediaLiveFrame::onBtnClicked, this, std::placeholders::_1));
    }

    loadVideoDeviceList();
}

bool CSJMediaLiveFrame::onBtnClicked(ui::EventArgs * args) {
    if (args->pSender == m_pCaptureBackBtn) {
        return true;
    }
    return false;
}

void CSJMediaLiveFrame::loadVideoDeviceList() {
    if (!m_pVideoDevcieCombo) {
        m_pVideoDevcieCombo = dynamic_cast<ui::Combo *>((getManager()->FindControl(L"CameraDeviceCombo")));
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
        m_pVideoFmtCombo = dynamic_cast<ui::Combo *>((getManager()->FindControl(L"CameraFmtCombo")));
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
        m_pVideoResolutionCombo = dynamic_cast<ui::Combo *>((getManager()->FindControl(L"CameraResolutionCombo")));
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

ui::ListContainerElement * CSJMediaLiveFrame::createListElement(std::wstring & text, 
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
    listElement->SetFixedHeight(fixedHeight > 0 ? fixedHeight : DUI_LENGTH_AUTO);
    listElement->SetBkColor(bkcolor);
    listElement->SetSelectedStateColor(ui::kControlStateNormal, selectedcolor);

    return listElement;
}
