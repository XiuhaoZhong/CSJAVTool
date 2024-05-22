#pragma once

#include "base/base.h"
#include "duilib/UIlib.h"

/**
 * @brief This class provides the functionality to custome ui modules;
 *
 * Users can this class create the ui modules, which can make the UI become more 
 * modularize so that users can divide total UI into many parts, that is, it's
 * very efficient and clear to build the UI logics.
 */
class CSJToolModuleUIBase : public nbase::SupportWeakCallback {
public:
    CSJToolModuleUIBase(ui::STRINGorID xml, ui::Window *manager, ui::Box *parent);
    virtual ~CSJToolModuleUIBase();

    /**
     * @brief show or hide the module UI.
     */
    virtual void showFrame(bool bshow);

    /**
     * @brief ui components initialize.
     */
    virtual void initUI();

    ui::Window* getManager() const {
        return m_pManager;
    }

    ui::Box* getParent() const {
        return m_pParent;
    }

    ui::Box* getRoot() const {
        return m_pRoot;
    }

protected:

private:
    ui::Window *m_pManager;
    ui::Box    *m_pParent;
    ui::Box    *m_pRoot;    

};

