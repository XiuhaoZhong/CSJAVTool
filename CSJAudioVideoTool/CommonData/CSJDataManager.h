#pragma once

#include <memory>
#include <windows.h>

#include "base/memory/singleton.h"

class CSJDataManager /*: public nbase::Singleton<CSJDataManager>*/ {
public:
	SINGLETON_DEFINE(CSJDataManager);

	CSJDataManager();
	~CSJDataManager();
	
	void setMainHwnd(HWND mainHwnd);
	HWND getMainHwnd() const;

	void setRenderHwnd(HWND renderHwnd);
	HWND getRenderHwnd() const;

protected:

private:
	HWND main_hWnd;		// �����ھ��;
	HWND render_hWnd;	// ��Ⱦ���ھ��;
};


