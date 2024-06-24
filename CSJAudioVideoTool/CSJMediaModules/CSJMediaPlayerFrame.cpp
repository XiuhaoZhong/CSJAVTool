#include "CSJMediaPlayerFrame.h"

#include <shobjidl_core.h>
//#include <ShlObj.h>
#include <CommCtrl.h>
#include <atlbase.h>
#include <commdlg.h>

#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>


#include "CSJDataManager.h"
#include "CSJMFPlayer/CSJMFPlayer.h"

using namespace ui;

// This function isn't used
static int CALLBACK fileChooseCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_IUNKNOWN) {
        CComPtr<IFolderFilterSite> folderFilterSite = NULL;
        HRESULT hr = ((IUnknown *)lParam)->QueryInterface(IID_PPV_ARGS(&folderFilterSite));
        if (FAILED(hr)) {
            return 0;
        }
    } else if (uMsg == BFFM_INITIALIZED) {
        // Set the text for the ok button.
        //SendMessage(hWnd, BFFM_SETOKTEXT, NULL, (LPARAM)L"Choose");
    }
    return 0;
}

CSJMediaPlayerFrame::CSJMediaPlayerFrame(ui::STRINGorID & xml, ui::Window * manager, ui::Box * parent)
    : CSJUIModuleBase(xml, manager, parent) {

}

CSJMediaPlayerFrame::~CSJMediaPlayerFrame() {

}

void CSJMediaPlayerFrame::initUI() {
    m_pPlayBtn = dynamic_cast<Button *>(getManager()->FindControl(L"PlayBtn"));
    if (m_pPlayBtn) {
        m_pPlayBtn->AttachClick(nbase::Bind(&CSJMediaPlayerFrame::onBtnClicked, this, std::placeholders::_1));
    }

    m_pPauseBtn = dynamic_cast<Button *>(getManager()->FindControl(L"PauseBtn"));
    if (m_pPauseBtn) {
        m_pPauseBtn->AttachClick(nbase::Bind(&CSJMediaPlayerFrame::onBtnClicked, this, std::placeholders::_1));
    }

    m_pSelectBtn = dynamic_cast<Button *>(getManager()->FindControl(L"SelectBtn"));
    if (m_pSelectBtn) {
        m_pSelectBtn->AttachClick(nbase::Bind(&CSJMediaPlayerFrame::onBtnClicked, this, std::placeholders::_1));
    }

    m_pBackBtn = dynamic_cast<Button *>(getManager()->FindControl(L"BackBtn"));
    if (m_pBackBtn) {
        m_pBackBtn->AttachClick(nbase::Bind(&CSJMediaPlayerFrame::onBtnClicked, this, std::placeholders::_1));
    }

    m_pSelectLbl = dynamic_cast<Label *>(getManager()->FindControl(L"SelectItemLbl"));
}

bool CSJMediaPlayerFrame::onBtnClicked(ui::EventArgs * args) {
    if (args->pSender == m_pPlayBtn) {
        onPlayBtnClicked();
    } else if (args->pSender == m_pBackBtn) {
        onBackBtnClicked();
    } else if (args->pSender == m_pPauseBtn) {
        onPauseBtnClicked();
    } else if (args->pSender == m_pSelectBtn) {
        onSelectBtnClicked();
    } else {
    
    }

    return true;
}

void CSJMediaPlayerFrame::onPlayBtnClicked() {

    CSJSpMFPlayer mfPlayer = CSJMFPlayer::generateMFPlayer();

    std::wstring path(L"partyAnimals.mp4");//Macroform_-_Simplicity.mp3 big_buck_bunny.mp4
    //std::wstring path(L"big_buck_bunny.mp4");
    mfPlayer->setPlayFile(path);
    mfPlayer->start();
}

void CSJMediaPlayerFrame::onPauseBtnClicked() {
}

void CSJMediaPlayerFrame::onBackBtnClicked() {

}

void CSJMediaPlayerFrame::onSelectBtnClicked() {

    int fileDialogType = 1;

    if (fileDialogType == 0) {
        LPITEMIDLIST pi1 = NULL;
        INITCOMMONCONTROLSEX InitCtrls = { 0 };
        TCHAR szBuf[4096] = { 0 };
        BROWSEINFO bi = { 0 };
        bi.hwndOwner = CSJDataManager::GetInstance()->getMainHwnd();
        bi.iImage = 0;
        bi.lParam = NULL;
        bi.lpfn = fileChooseCallback;
        bi.lpszTitle = _T("Choose a file to play");
        bi.pszDisplayName = szBuf;
        bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_USENEWUI;

        InitCommonControlsEx(&InitCtrls);
        pi1 = SHBrowseForFolder(&bi);
        if (pi1) {
            SHGetPathFromIDList(pi1, szBuf);
            std::wstring selectFile(szBuf);
            m_playFilePath = selectFile;

            if (m_pSelectLbl) {
                m_pSelectLbl->SetText(m_playFilePath);
            }
        }
    } else if (fileDialogType == 1) {
        TCHAR szFilePath[1024] = {};
        OPENFILENAME ofn = { 0 };
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = CSJDataManager::GetInstance()->getMainHwnd();
        std::wstring fileStr(L"");
        ofn.lpstrFilter = (LPWSTR)L"Media Files(*.mp4;*.mp3;*.3gp;*.AVI)\0*.mp4;*.mp3;*.3gp;*.AVI\0\0";
        ofn.lpstrInitialDir = L"./";
        ofn.lpstrFile = szFilePath;
        ofn.nMaxFile = sizeof(szFilePath) / sizeof(*szFilePath);
        ofn.nFilterIndex = 0;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
        if (!GetOpenFileName(&ofn)) {
            return ;
        }
        
        std::wstring result(szFilePath);
        if (result.size() > 0) {
            m_pSelectLbl->SetText(result);
        }
    }
}
