#ifndef __CSJMEDIALIVEFRAME_H__
#define __CSJMEDIALIVEFRAME_H__

#include "main/CSJToolModuleUIBase.h"

class CSJMediaLiveFrame : public CSJToolModuleUIBase {
public:
    CSJMediaLiveFrame();

    CSJMediaLiveFrame(ui::STRINGorID &xml, ui::Window *manager, ui::Box *parent);
    ~CSJMediaLiveFrame();

    void initUI() override;
protected:
    bool onBtnClicked(ui::EventArgs *args);
    /**
     * @brief load capture devices, including video and audio capture.
     */
    void loadVideoDeviceList();

    /**
     * @brief create list element with the params.
     *
     * @param[in] text          the text the element show.
     * @param[in] index         the index of current element.
     * @param[in] fixedWidth    the fixed width of the element, if users want to
                                auto estimating by system, pass 0.
     * @param[in] fixedHeight   the fixed height of the element, pass 0 as same
                                with fixedWidth.

     * @param[in] bkcolor       the background color of current element, default is white.
     * @param[in] selectedcolor the background of selected status, default is blue.
     *
     * @return    if success return a pointer points the list element, or return nullptr.
     */
    ui::ListContainerElement* createListElement(std::wstring &text,
                                                int index,
                                                int fixedWidth,
                                                int fixedHeight,
                                                std::wstring bkcolor = L"white",
                                                std::wstring selectedcolor = L"blue");

private:
    // ui components.
    ui::HBox    *m_pCaptureBox           = nullptr;
    ui::Button  *m_pCaptureBackBtn       = nullptr;
    ui::Combo   *m_pVideoDevcieCombo     = nullptr;
    ui::Combo   *m_pVideoFmtCombo        = nullptr;
    ui::Combo   *m_pVideoResolutionCombo = nullptr;
};

#endif // __CSJMEDIALIVEFRAME_H__