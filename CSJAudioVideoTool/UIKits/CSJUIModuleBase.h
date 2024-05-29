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
class CSJUIModuleBase : public nbase::SupportWeakCallback {
public:
    CSJUIModuleBase(ui::STRINGorID xml, ui::Window *manager, ui::Box *parent);
    virtual ~CSJUIModuleBase();

    /**
     * This delegate provides functions are used to module UIs communicates its' parent. 
     *
     * When a module UI is showing in the whole area of it's parent, it is necessary to 
     * notify it's parent when current ui will hide/ destory or other case that the parent
     * can't response immediately.
     */
    class Delegate {
    public:
        Delegate() = default;
        ~Delegate() = default;

        /**
         * Notify parent that current module will return.
         */
        virtual void onModuleBack() = 0;
    };

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

