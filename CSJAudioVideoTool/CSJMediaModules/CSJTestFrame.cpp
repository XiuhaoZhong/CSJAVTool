#include "CSJTestFrame.h"

using namespace ui;

CSJTestFrame::CSJTestFrame(ui::STRINGorID & xml, ui::Window * manager, ui::Box * parent)
    : CSJUIModuleBase(xml, manager, parent) {

}

CSJTestFrame::~CSJTestFrame() {

}

void CSJTestFrame::initUI() {
    m_pDialogBtn = dynamic_cast<Button *>(getManager()->FindControl(L"DialogBtn"));
    if (m_pDialogBtn) {
        m_pDialogBtn->AttachClick(nbase::Bind(&CSJTestFrame::onBtnClicked, this, std::placeholders::_1));
    }
    m_pBackBtn = dynamic_cast<Button *>(getManager()->FindControl(L"BackBtn"));
    if (m_pBackBtn) {
        m_pBackBtn->AttachClick(nbase::Bind(&CSJTestFrame::onBtnClicked, this, std::placeholders::_1));
    }
}

bool CSJTestFrame::onBtnClicked(ui::EventArgs * args) {
    if (args->pSender == m_pDialogBtn) {
        onDialogBtnClicked();
    } else if (args->pSender == m_pBackBtn) {
        onBackBtnClicked();
    }

    return true;
}

void CSJTestFrame::onDialogBtnClicked() {

}

void CSJTestFrame::onBackBtnClicked() {

}
