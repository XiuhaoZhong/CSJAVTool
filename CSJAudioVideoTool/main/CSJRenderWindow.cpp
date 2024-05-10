#include "CSJRenderWindow.h"

#include <iostream>

#include "CSJDataManager.h"

using namespace nim_comp;

//void testForPtr() {
//    std::unique_ptr<CSJDataManager> up;
//
//    std::shared_ptr<CSJDataManager> sp;
//
//    std::weak_ptr<CSJDataManager> wp;
//}

std::shared_ptr<CSJRenderWindow> CSJRenderWindow::render_window_ = nullptr;

std::shared_ptr<CSJRenderWindow> CSJRenderWindow::getInstance() {
    if (!render_window_) {
        render_window_ = std::make_shared<CSJRenderWindow>();
    }

    return render_window_;
}

CSJRenderWindow::CSJRenderWindow()
    : m_leftDelta(0)
    , m_topDelta(0)
    , m_width(0)
    , m_height(0) {

}

CSJRenderWindow::CSJRenderWindow(int leftDelta, int topDelta, int width, int height)
    : m_leftDelta(leftDelta)
    , m_topDelta(topDelta)
    , m_width(width)
    , m_height(height) {
}

CSJRenderWindow::~CSJRenderWindow() {
    
}

std::wstring CSJRenderWindow::GetWindowClassName() const {
    return L"CSJRenderWindow";
}

std::wstring CSJRenderWindow::GetWindowId() const {
    return L"";
}

std::wstring CSJRenderWindow::GetSkinFolder() {
    return L"basic";
}

std::wstring CSJRenderWindow::GetSkinFile() {
    return L"csj_render_window.xml";
}

HWND CSJRenderWindow::createRenderWindow(HWND hwndParent,
                                         const ui::UiRect& rc) {
    std::wstring className = GetWindowClassName();

    m_hWnd = Create(hwndParent, L"CSJRenderWindow", WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP, WS_EX_TOOLWINDOW, false);
    LONG errorCode = ::GetLastError();
    if (errorCode == ERROR_CLASS_ALREADY_EXISTS) {
        std::cout << "" << std::endl;
    }

    LONG nWindowLong = GetWindowLong(m_hWnd, GWL_STYLE);
    if (nWindowLong & WS_CAPTION) {
        SetWindowLong(m_hWnd, GWL_STYLE, nWindowLong & ~WS_CAPTION);
    }

    ASSERT(m_hWnd != NULL);
    return m_hWnd;
}

void CSJRenderWindow::CloseWindow() {
    if (m_renderMgr) {
        m_renderMgr->stopRendering();
    }

    PostMessage(WM_CLOSE, (WPARAM)0, 0L);
}

LONG CSJRenderWindow::GetStyle() {
    LONG styleValue = ::GetWindowLong(GetHWND(), GWL_STYLE);
    return styleValue;
}

void CSJRenderWindow::InitWindow() {
    m_renderMgr = CSJGLRenderManager::getDefaultRenderManager();
    m_renderMgr->initGL(m_hWnd, m_width, m_height);
}

void CSJRenderWindow::OnCreate() {
    
}

/*
void CSJRenderWindow::initializePos(HWND pHwnd) {
    if (!pHwnd) {
        return;
    }

    RECT rc;
    memset(&rc, 0, sizeof(RECT));

    ::GetWindowRect(pHwnd, &rc);
    bool res = SetWindowPos(m_hWnd, pHwnd, rc.left + m_leftDelta, rc.top + m_topDelta, m_width, m_height, SWP_NOZORDER);
    if (res) {
        std::cout << "Initialize render window position successfully" << std::endl;
    } else {
        std::cout << "Could not initialize render window position." << std::endl;
    }

    RECT rect_mask;
    ::GetWindowRect(m_hWnd, &rect_mask);

    int top = rect_mask.top;
    int left = rect_mask.left;
    int bottom = rect_mask.bottom;
    int right = rect_mask.right;

    std::cout << "Render widow rect: " << top << ", " << left << ", " << bottom
        << ", " << right << std::endl;
}
*/

LRESULT CSJRenderWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (m_renderMgr) {
        m_renderMgr->stopRendering();
    }

    ::DestroyWindow(m_hWnd);
    return 0;
}
