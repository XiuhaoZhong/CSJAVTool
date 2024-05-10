#pragma once

#include <memory.h>

#include "ui_components/windows_manager/window_ex.h"
#include "CSJGLRenderManager/CSJGLRenderManager.h"

class CSJRenderWindow : public nim_comp::WindowEx {
public:
    CSJRenderWindow();
    CSJRenderWindow(int leftDelta, int topDelta, int width, int height);
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
    int                   m_leftDelta;
    int                   m_topDelta;
    int                   m_width;
    int                   m_height;
    std::wstring          m_clsName;
    CSJSharedRenderManger m_renderMgr;

    static std::shared_ptr<CSJRenderWindow> render_window_;
};

using sharedRenderWidow = std::shared_ptr<CSJRenderWindow>;
