#include "CSJRenderWindow.h"

#include <iostream>

#include "CSJDataManager.h"
#include "RenderManager.h"

using namespace nim_comp;

void testForPtr() {
    std::unique_ptr<CSJDataManager> up;

    std::shared_ptr<CSJDataManager> sp;

    std::weak_ptr<CSJDataManager> wp;
}

std::shared_ptr<CSJRenderWindow> CSJRenderWindow::render_window_ = nullptr;

std::shared_ptr<CSJRenderWindow> CSJRenderWindow::getInstance() {
    if (!render_window_) {
        render_window_ = std::make_shared<CSJRenderWindow>();
    }

    return render_window_;
}

CSJRenderWindow::CSJRenderWindow() {

}

CSJRenderWindow::~CSJRenderWindow() {}

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
    RenderManager::GetInstance()->stop();
}

LONG CSJRenderWindow::GetStyle() {
    LONG styleValue = ::GetWindowLong(GetHWND(), GWL_STYLE);
    return styleValue;
}

void CSJRenderWindow::InitWindow() {
    CSJDataManager::GetInstance()->setRenderHwnd(m_hWnd);
    RenderManager::GetInstance()->initRenderEnv();
}

bool CSJRenderWindow::registerWindowClass() {
    WNDCLASS wc = { 0 };
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = CSJRenderWindow::__WndProc;
    wc.hInstance = ::GetModuleHandle(NULL);
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    std::wstring className = GetWindowClassName();
    wc.lpszClassName = className.c_str();
    ATOM ret = ::RegisterClass(&wc);
    LONG errorCode = ::GetLastError();
    if (errorCode == ERROR_CLASS_ALREADY_EXISTS) {
        std::cout << "" << std::endl;
    }

    ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

void CSJRenderWindow::OnCreate() {
    ::ShowWindow(m_hWnd, true);

    CSJDataManager::GetInstance()->setRenderHwnd(m_hWnd);
    RenderManager::GetInstance()->initRenderEnv();
}

void CSJRenderWindow::initializePos(HWND pHwnd) {
    if (!pHwnd) {
        return;
    }

    RECT rc;
    memset(&rc, 0, sizeof(RECT));

    ::GetWindowRect(pHwnd, &rc);

    bool res = SetWindowPos(m_hWnd, pHwnd, rc.left + 50, rc.top + 50, 400, 300, SWP_NOZORDER);
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

LRESULT CSJRenderWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    ::DestroyWindow(m_hWnd);
    return 0;
}

/*
LRESULT CSJRenderWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    ::PostQuitMessage(0);
    return 0;
}
*/