#include "GLRenderImpl.h"

#include <windows.h>
#include "GL/glew.h"

#include "CSJDataManager.h"
#include "gl_display.h"
#include "Render.h"

SenceRenderInfo::SenceRenderInfo() {

}

SenceRenderInfo::~SenceRenderInfo() {

}

HWND SenceRenderInfo::GetHwnd() {
	return CSJDataManager::GetInstance()->getRenderHwnd();
}

GLRenderImpl::GLRenderImpl() {
	gl_device_.plat = nullptr;
	gl_device_.cur_swap = nullptr;

	render_thread_ = INVALID_HANDLE_VALUE;
	render_thread_id_ = -1;

	start_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	pause_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	stop_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	out_video_buf_ = std::make_shared<CSJOutVideoBuf>();
	out_video_buf_->Initial(1, 800 * 600 * 4, nullptr);
}

GLRenderImpl::~GLRenderImpl() {

}

void GLRenderImpl::Init() {
	if (render_thread_ == INVALID_HANDLE_VALUE) {
		render_thread_ = CreateThread(NULL, 0, ProcessWndProc, this, 0, &render_thread_id_);
	}

	WaitForSingleObject(start_event_, INFINITE);

	if (opengl_initialized_) {
		opengl_initialized_();
	}

	if (WaitForSingleObject(start_event_, INFINITE) == WAIT_OBJECT_0) {
		//PostProcessorMsg(WM_INITOPENGL);
	}

	initRenderDevice();

	CloseHandle(start_event_);
	start_event_ = NULL;
}

void GLRenderImpl::setEnvCreatedFunc(std::function<void()> func) {
	opengl_initialized_ = func;
}

bool GLRenderImpl::InitOpenGL() {
	__try {
		gl_device_.plat = gl_platform_create(&gl_device_, 0);
		if (gl_device_.plat) {
			GLint max_texture_size = 0;
			glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &max_texture_size);
			int major = 0, minor = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			return true;
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		// OpenGL initialize failed!
	}

	return false;
}

void GLRenderImpl::initRenderDevice() {
	sence_render_info_ = std::make_shared<SenceRenderInfo>();

    // sence_render_info_->GetHwnd() 获取的是渲染窗口的句柄;
    // gl_windowinfo_create() 返回的就是渲染窗口的渲染属性;
	display_.window_info_ = gl_windowinfo_create(sence_render_info_->GetHwnd()); 
	display_.draw_callback_ = std::make_shared<Render>();
	display_.out_buf_ = out_video_buf_;
	display_.render_info_ = sence_render_info_;

    // display_ 存储的是渲染窗口(CSJRenderWindow)的渲染属性;
}

void GLRenderImpl::Process() {
	std::lock_guard<std::recursive_mutex> lock(displays_mutex_);

	if (!display_.out_buf_) {
		return ;
	}

	gl_device_.cur_swap = display_.window_info_;
	gl_enter_context(&gl_device_);

	display_.draw_callback_->Draw(display_.render_info_, display_.out_buf_);
	wglSwapLayerBuffers(display_.window_info_->hdc, WGL_SWAP_MAIN_PLANE);

	gl_leave_context(&gl_device_);
}

void GLRenderImpl::StopRender() {
    if (stop_event_) {
        ::SetEvent(stop_event_);
    }
}

DWORD GLRenderImpl::ProcessWndProc(LPVOID p) {
	//MSG msg;
	//int nRet = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

	bool bInit = GetInstance()->InitOpenGL();

	if (!bInit) {
		// OpenGL initialize failed;
		return 0;
	}

	::SetEvent(GetInstance()->start_event_);

	HANDLE pEvents[2];
	pEvents[0] = GetInstance()->stop_event_;
	pEvents[1] = GetInstance()->pause_event_;

	while (true) {
		DWORD dwObj = WaitForMultipleObjects(2, pEvents, FALSE, 0);

		if (dwObj == WAIT_OBJECT_0) {
			// 结束绘制;
			break;
		} else if (dwObj == WAIT_OBJECT_0 + 1) {
			// 暂停绘制
			continue;
		}

		GetInstance()->Process();
	}

	return 0;
}