#include "CSJToolModuleUIBase.h"

#include "duilib/Core/WindowBuilder.h"

using namespace ui;

CSJToolModuleUIBase::CSJToolModuleUIBase(STRINGorID xml, Window* manager, Box* parent) {
    ui::WindowBuilder windowBuilder;
    m_pRoot = windowBuilder.Create(xml, ui::CreateControlCallback(), manager, parent, nullptr);
    if (m_pRoot) {
        m_pManager = manager;
        m_pParent = parent;

        //initUI();
    }
}

CSJToolModuleUIBase::~CSJToolModuleUIBase() {

}

void CSJToolModuleUIBase::showFrame(bool bshow) {
    if (!m_pRoot) {
        return;
    }

    m_pRoot->SetVisible(bshow);
}

void CSJToolModuleUIBase::initUI() {

}
