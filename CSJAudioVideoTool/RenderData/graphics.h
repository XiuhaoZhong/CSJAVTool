#pragma once

#include <windows.h>
#include <stdint.h>

struct gl_windowinfo {
	HWND hwnd;
	HDC hdc;
};

struct gl_platform {
	HGLRC hrc;
	struct gl_windowinfo window;
};

struct gl_context {
	HWND hwnd;
	HGLRC hrc;
	HDC hdc;
};

struct gl_device {
	struct gl_platform *plat;
	gl_windowinfo	   *cur_swap;
};

extern struct gl_platform	*gl_platform_create(gl_device *device, uint32_t adapter);
extern void					 gl_platform_destroy(struct gl_platform *platform);
extern bool					 gl_enter_context(gl_device *device);
extern void					 gl_leave_context(gl_device *device);

extern struct gl_windowinfo *gl_windowinfo_create(HWND hwnd);
extern void					 gl_windowinfo_destroy(struct gl_windowinfo *wi);