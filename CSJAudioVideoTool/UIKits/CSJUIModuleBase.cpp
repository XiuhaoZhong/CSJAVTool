#include "CSJUIModuleBase.h"

#include "duilib/Core/WindowBuilder.h"

using namespace ui;

CSJUIModuleBase::CSJUIModuleBase(STRINGorID xml, Window* manager, Box* parent) {
    ui::WindowBuilder windowBuilder;
    m_pRoot = windowBuilder.Create(xml, ui::CreateControlCallback(), manager, parent, nullptr);
    if (m_pRoot) {
        m_pManager = manager;
        m_pParent = parent;

        //initUI();
    }
}

CSJUIModuleBase::~CSJUIModuleBase() {

}

void CSJUIModuleBase::showFrame(bool bshow) {
    if (!m_pRoot) {
        return;
    }

    m_pRoot->SetVisible(bshow);
}

void CSJUIModuleBase::initUI() {

}
