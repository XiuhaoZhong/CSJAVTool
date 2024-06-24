#ifndef __CSJMEDIAPLAYERFRAME_H__
#define __CSJMEDIAPLAYERFRAME_H__

#include "UIKits/CSJUIModuleBase.h"

class CSJMediaPlayerFrame : public CSJUIModuleBase {
public:
    CSJMediaPlayerFrame(ui::STRINGorID &xml, ui::Window *manager, ui::Box *parent);
    ~CSJMediaPlayerFrame();

    void initUI() override;

protected:
    bool onBtnClicked(ui::EventArgs *args);

    void onPlayBtnClicked();
    void onPauseBtnClicked();
    void onBackBtnClicked();
    void onSelectBtnClicked();

private:
    std::wstring m_playFilePath;

    ui::Button *m_pPlayBtn = nullptr;
    ui::Button *m_pPauseBtn = nullptr;
    ui::Button *m_pSelectBtn = nullptr;
    ui::Button *m_pBackBtn = nullptr;
    ui::Label  *m_pSelectLbl = nullptr;


};

#endif // __CSJTESTFRAME_H__