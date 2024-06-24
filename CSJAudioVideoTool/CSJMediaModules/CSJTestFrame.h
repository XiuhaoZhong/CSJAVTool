#ifndef __CSJTESTFRAME_H__
#define __CSJTESTFRAME_H__

#include "UIKits/CSJUIModuleBase.h"

class CSJTestFrame : public CSJUIModuleBase {
public:
    CSJTestFrame(ui::STRINGorID &xml, ui::Window *manager, ui::Box *parent);
    ~CSJTestFrame();

    void initUI() override;

protected:
    bool onBtnClicked(ui::EventArgs *args);

    void onDialogBtnClicked();
    void onBackBtnClicked();

private:
    ui::Button *m_pDialogBtn = nullptr;
    ui::Button *m_pBackBtn   = nullptr;

};

#endif // __CSJTESTFRAME_H__