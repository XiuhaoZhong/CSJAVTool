//#include "stdafx.h"

#include "main.h"

#include <string>
#include <mfapi.h>

#include "duilib/UIlib.h"

#include "base/thread/thread_manager.h"
#include "base/win32/path_util.h"

#include "duilib/Core/GlobalManager.h"
#include "duilib/core/WindowBuilder.h"

#include "ToolWindow.h"


enum Thread {
	kThreadUI
};

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance, 
					  _In_ LPWSTR lpCmdLine, 
					  _In_ int nShowCmd ) {
	// 创建主线程;
	MainThread thread;

	// 执行主线程循环;
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	return 0;
}

void MainThread::Init() {
	nbase::ThreadManager::RegisterThread(kThreadUI);

	std::wstring themeDir = nbase::win32::GetCurrentModuleDirectory();

#ifdef _DEBUG
	ui::GlobalManager::Startup(themeDir + L"resources\\", ui::CreateControlCallback(), false);
#else
#endif 
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        // Com initialize failed;
    }

    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
       // MFStartup initialize failed.
    }

	ToolWindow* window = new ToolWindow();
	window->Create(NULL, window->kClassName.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	window->CenterWindow();
	window->ShowWindow();

	window->createRenderWindow();
}

void MainThread::Cleanup() {
	ui::GlobalManager::Shutdown();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
    MFShutdown();
    CoUninitialize();
}