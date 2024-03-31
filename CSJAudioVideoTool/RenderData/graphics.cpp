#include "graphics.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#define DUMMY_WNDCLASS L"Dummy GL Window Class"

static const wchar_t *dummy_window_class = L"GLDummyWindow";
static bool registered_dummy_window_class = false;

struct dummy_context {
	HWND hwnd;
	HGLRC hrc;
	HDC   hdc;
};

static inline HGLRC gl_init_basic_context(HDC hdc) {
	HGLRC hglrc = wglCreateContext(hdc);
	if (!hglrc) {
		return NULL;
	}

	if (!wglMakeCurrent(hdc, hglrc)) {
		wglDeleteContext(hglrc);
		return NULL;
	}

	return hglrc;
}

static bool gl_dummy_context_init(struct dummy_context *dummy) {
	WNDCLASS wc;
	if (registered_dummy_window_class) {
		return true;
	}

	memset(&wc, 0, sizeof(wc));
	wc.style = CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = dummy_window_class;

	if (!RegisterClass(&wc)) {
		// couldn't create dummy window class.
		return false;
	}

	HWND hwnd = CreateWindowEx(0, dummy_window_class, L"Dummy GL Window",
							   WS_POPUP,
							   0, 0, 2, 2,
							   NULL, NULL, GetModuleHandle(NULL), NULL);
	if (!hwnd) {
		return false;
	}

	dummy->hwnd = hwnd;
	dummy->hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.cStencilBits = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	int format_index = ChoosePixelFormat(dummy->hdc, &pfd);
	if (!format_index) {
		return false;
	}

	if (!SetPixelFormat(dummy->hdc, format_index, &pfd)) {
		return false;
	}

	dummy->hrc = gl_init_basic_context(dummy->hdc);
	if (!dummy->hrc) {
		return false;
	}

	return true;
}

static bool gl_init_extensions(HDC hdc) {
	if (glewInit() == 0) {
		return true;
	}

	return false;
}

static bool register_dummy_class(void) {
	static bool created = false;

	WNDCLASS wc = {0};
	wc.style = CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = (WNDPROC)DefWindowProcA;
	wc.lpszClassName = DUMMY_WNDCLASS;

	if (created) {
		return true;
	}

	if (!RegisterClass(&wc)) {
		return false;
	}

	created = true;
	return true;
}

static bool create_dummy_window(struct gl_platform *plat) {
	plat->window.hwnd = CreateWindowEx(0, DUMMY_WNDCLASS,
									   L"OpenGL Dummy Window", WS_POPUP, 0, 0, 1, 1,
									   NULL, NULL, GetModuleHandle(NULL), NULL);

	if (!plat->window.hwnd) {
		return false;
	}

	plat->window.hdc = GetDC(plat->window.hwnd);
	if (!plat->window.hdc) {
		return false;
	}

	return true;
}

static inline bool gl_getpixelformat(HDC hdc, int *format, PIXELFORMATDESCRIPTOR *pfd) {
	if (!format) {
		return false;
	}

	// Specify the important attributes we care about;
	int pixAttribs[] = {
		WGL_SUPPORT_OPENGL_ARB, 1, // Must support OGL redering; 
		WGL_DRAW_TO_WINDOW_ARB, 1, // pf that can run a window;
		WGL_ACCELERATION_ARB,
		WGL_FULL_ACCELERATION_ARB,
		WGL_DOUBLE_BUFFER_ARB,
		GL_TRUE,
		WGL_COLOR_BITS_ARB, 32, // 8 bits of red precision in window;
		WGL_DEPTH_BITS_ARB, 0,  // 8 bits of green precision in window;
		WGL_STENCIL_BITS_ARB, 0, // 16 bits of depth precision for window;
		WGL_PIXEL_TYPE_ARB, 
		WGL_TYPE_RGBA_ARB, // pf should be RGBA type;
		0
	};

	int nPixCount;
	BOOL success = wglChoosePixelFormatARB(hdc, &pixAttribs[0], NULL, 1, format, (UINT*)&nPixCount);

	if (!success || !nPixCount) {
		*format = 0;
	}

	if (!DescribePixelFormat(hdc, *format, sizeof(*pfd), pfd)) {
		return false;
	}

	return true;
}

static inline bool gl_setpixelformat(HDC hdc, int format, PIXELFORMATDESCRIPTOR *pfd) {
	if (!SetPixelFormat(hdc, format, pfd)) {
		return false;
	}

	return true;
}

static inline void gl_dummy_context_free(struct dummy_context *dummy) {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(dummy->hrc);
	DestroyWindow(dummy->hwnd);
	memset(dummy, 0, sizeof(struct dummy_context));
}

static inline HGLRC gl_init_context(HDC hdc) {
	return gl_init_basic_context(hdc);
}

void gl_platform_destroy(struct gl_platform *plat) {
	if (plat) {
		if (plat->hrc) {
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(plat->hrc);
		}

		if (plat->window.hdc) {
			ReleaseDC(plat->window.hwnd, plat->window.hdc);
		}

		if (plat->window.hwnd) {
			DestroyWindow(plat->window.hwnd);
		}

		free(plat);
	}
}

struct gl_platform* gl_platform_create(gl_device *device, uint32_t adapter) {
	struct gl_platform *plat = (gl_platform *)malloc(sizeof(struct gl_platform));
	struct dummy_context dummy;

	int pixel_format;
	PIXELFORMATDESCRIPTOR pfd;

	memset(&dummy, 0, sizeof(struct dummy_context));

	if (!gl_dummy_context_init(&dummy)) {
		goto fail;
	}

	if (!gl_init_extensions(dummy.hdc)) {
		goto fail;
	}

	if (!register_dummy_class()) {
		return NULL;
	}

	if (!create_dummy_window(plat)) {
		return NULL;
	}

	/**
	 * you have to have a dummy context open before you can actually
	   use wglChoosePixelFormatARB.
	 */
	if (!gl_getpixelformat(dummy.hdc, &pixel_format, &pfd)) {
		goto fail;
	}

	gl_dummy_context_free(&dummy);

	if (!gl_setpixelformat(plat->window.hdc, pixel_format, &pfd)) {
		goto fail;
	}

	plat->hrc = gl_init_context(plat->window.hdc);
	if (!plat->hrc) {
		goto fail;
	}

	return plat;

fail:
	gl_platform_destroy(plat);
	gl_dummy_context_free(&dummy);
	return NULL;
}

void gl_windowinfo_destroy(struct gl_windowinfo *wi) {
	if (wi) {
		if (wi->hdc) {
			ReleaseDC(wi->hwnd, wi->hdc);
		}

		free(wi);
	}
}

struct gl_windowinfo* gl_windowinfo_create(HWND hwnd) {
	struct gl_windowinfo *wi = (struct gl_windowinfo*)malloc(sizeof(struct gl_windowinfo));
	wi->hwnd = hwnd;
	wi->hdc = GetDC(wi->hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	int pixel_format;

	if (!wi) {
		return NULL;
	}

	if (!gl_getpixelformat(wi->hdc, &pixel_format, &pfd)) {
		goto fail;
	}

	if (!gl_setpixelformat(wi->hdc, pixel_format, &pfd)) {
		goto fail;
	}

	return wi;

fail:
	gl_windowinfo_destroy(wi);
	return NULL;
}

bool gl_enter_context(gl_device *device) {
	HDC hdc = device->plat->window.hdc;
	if (device->cur_swap) {
		hdc = device->cur_swap->hdc;
	}

	if (!wglMakeCurrent(hdc, device->plat->hrc)) {
		DWORD dw = GetLastError();

		return false;
	}

	return true;
}

void gl_leave_context(gl_device *device) {
	wglMakeCurrent(NULL, NULL);
}