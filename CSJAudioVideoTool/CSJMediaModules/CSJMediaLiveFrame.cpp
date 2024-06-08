#include "CSJMediaLiveFrame.h"

#include <string>

using namespace ui;

CSJMediaLiveFrame::CSJMediaLiveFrame()
    : CSJUIModuleBase(L"", nullptr, nullptr) {
}

CSJMediaLiveFrame::CSJMediaLiveFrame(ui::STRINGorID & xml, ui::Window * manager, ui::Box * parent)
    : CSJUIModuleBase(xml, manager, parent) {

}

CSJMediaLiveFrame::~CSJMediaLiveFrame() {

}

void CSJMediaLiveFrame::initUI() {
    m_pCaptureBox = dynamic_cast<HBox *>((getManager()->FindControl(L"CaptureBox")));
    m_pCaptureBackBtn = dynamic_cast<Button *>((getManager()->FindControl(L"CaptureReturnBtn")));
    if (m_pCaptureBackBtn) {
        m_pCaptureBackBtn->AttachClick(nbase::Bind(&CSJMediaLiveFrame::onBtnClicked, this, std::placeholders::_1));
    }

    m_pCapConrolBtn = dynamic_cast<Button *>((getManager()->FindControl(L"CapControlBtn")));
    if (m_pCapConrolBtn) {
        m_pCapConrolBtn->AttachClick(nbase::Bind(&CSJMediaLiveFrame::onBtnClicked, this, std::placeholders::_1));
    }
    
    m_pLiveHandler = std::make_shared<CSJMediaLiveHandler>();
    if (m_pLiveHandler->initLiveHandler()) {

        m_pLiveHandler->getAudioCapDevNames(m_audioDevNames);
        m_pLiveHandler->getVideoCapDevNames(m_videoDevNames);
        m_pLiveHandler->getFormatsWithDevIndex(m_selVideoDevIndex, m_videoDevFormats);
        m_pLiveHandler->getResolutionsWithDevIndexAndFmtIndex(m_selVideoDevIndex, m_selVideoDevFmtIndex, m_videoDevResolutions);

        loadVideoDeviceList();

        m_pLiveHandler->startCapture();
    }
}

bool CSJMediaLiveFrame::onBtnClicked(ui::EventArgs * args) {
    if (args->pSender == m_pCaptureBackBtn) {
        return true;
    } else if (args->pSender == m_pCapConrolBtn) {

        if (!m_pLiveHandler->isCapturing()) {
            onControlCapture(false);
        } else {
            onControlCapture(true);
        }

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
    for (size_t i = 0; i < m_videoDevNames.size(); i++) {
        std::wstring text = m_videoDevNames[i];
        ui::ListContainerElement *element = createListElement(text, i, 100, 20);
        if (element) {
            m_pVideoDevcieCombo->Add(element);
        }
    }
    m_pVideoDevcieCombo->SelectItem(m_selVideoDevIndex);

    if (!m_pVideoFmtCombo) {
        m_pVideoFmtCombo = dynamic_cast<ui::Combo *>((getManager()->FindControl(L"CameraFmtCombo")));
        m_pVideoFmtCombo->SetPopupTop(false);
    }

    m_pVideoFmtCombo->RemoveAll();
    for (size_t i = 0; i < m_videoDevFormats.size(); i++) {
        ui::ListContainerElement *element = createListElement(m_videoDevFormats[i], i, 100, 20);
        if (element) {
            m_pVideoFmtCombo->Add(element);
        }
    }
    m_pVideoFmtCombo->SelectItem(m_selVideoDevFmtIndex);

    if (!m_pVideoResolutionCombo) {
        m_pVideoResolutionCombo = dynamic_cast<ui::Combo *>((getManager()->FindControl(L"CameraResolutionCombo")));
        m_pVideoResolutionCombo->SetPopupTop(false);
    }

    m_pVideoResolutionCombo->RemoveAll();
    for (size_t i = 0; i < m_videoDevResolutions.size(); i++) {
        ui::ListContainerElement *element = createListElement(m_videoDevResolutions[i], i, 100, 20);
        if (element) {
            m_pVideoResolutionCombo->Add(element);
        }
    }
    m_pVideoResolutionCombo->SelectItem(m_selVideoDevResotionIndex);
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

void CSJMediaLiveFrame::onControlCapture(bool captureCtrl) {
    if (!m_pLiveHandler) {
        return ;
    }

    if (captureCtrl) {
        m_pLiveHandler->startCapture();
    } else {
        m_pLiveHandler->stopHandler();
    }
}

