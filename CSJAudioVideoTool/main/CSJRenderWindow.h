#pragma once

#include <memory.h>

#include "ui_components/windows_manager/window_ex.h"
#include "CSJGLRenderManager/CSJGLRenderManager.h"

class CSJRenderWindow : public nim_comp::WindowEx {
public:
    CSJRenderWindow();
    ~CSJRenderWindow();

    void CloseWindow();

    virtual std::wstring GetWindowClassName(void) const override;
    virtual std::wstring GetWindowId(void) const override;

    HWND createRenderWindow(HWND hwndParent,
                            const ui::UiRect& rc = ui::UiRect(0, 0, 0, 0));

    virtual void InitWindow() override;

    virtual LONG GetStyle() override; 

    CSJRenderWindow(const CSJRenderWindow& renderWindoe) = delete;
    CSJRenderWindow(CSJRenderWindow&& renderWindow) = delete;

    static std::shared_ptr<CSJRenderWindow> getInstance();

    void initializePos(HWND pHwnd);

    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

protected:

    void OnCreate();

    /**
    * @brief 创建窗口时被调用，由子类实现用以获取窗口皮肤目录
    * @return 子类需实现并返回窗口皮肤目录
    */
    virtual std::wstring GetSkinFolder() override;

    /**
    * @brief 创建窗口时被调用，由子类实现用以获取窗口皮肤 XML 描述文件
    * @return 子类需实现并返回窗口皮肤 XML 描述文件
    */
    virtual std::wstring GetSkinFile() override;

 private:
    static std::shared_ptr<CSJRenderWindow> render_window_;

    std::wstring m_clsName;

    CSJSharedRenderManger m_renderMgr;
};

using sharedRenderWidow = std::shared_ptr<CSJRenderWindow>;
