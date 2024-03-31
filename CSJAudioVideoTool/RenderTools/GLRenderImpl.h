#pragma once

#include <windows.h>
#include "base\memory\singleton.h"

//#include "CSJOutVideoBuf.h"
#include "gl_display.h"

//struct gl_device;
//struct gl_display;

class SenceRenderInfo : public IRenderInfo {
public:
	SenceRenderInfo();
	~SenceRenderInfo();

	HWND GetHwnd() override;
};

class GLRenderImpl {
public:
	GLRenderImpl();
	~GLRenderImpl();

	SINGLETON_DEFINE(GLRenderImpl);

	void Init();

	void setEnvCreatedFunc(std::function<void()> func);

    void StopRender();

private:
	//  初始化OpenGL;
	bool InitOpenGL();

	// 初始化主窗口绘制设备;
	void initRenderDevice();

	void Process();

	static DWORD WINAPI ProcessWndProc(LPVOID p);

private:
	std::function<void()> opengl_initialized_ = nullptr;

	HANDLE					render_thread_;
	DWORD					render_thread_id_;

	gl_device				gl_device_; // opengl绘制;
	gl_display				display_;
	std::recursive_mutex	displays_mutex_;

	std::shared_ptr<SenceRenderInfo> sence_render_info_;
	std::shared_ptr<CSJOutVideoBuf> out_video_buf_;

	// 控制渲染的信号量;
	HANDLE video_sem_;

	HANDLE start_event_;
	HANDLE stop_event_;
	HANDLE pause_event_;
};