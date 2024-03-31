#pragma once

#include "graphics.h"
#include "CSJOutVideoBuf.h"

struct gl_display {
	std::shared_ptr<IRenderInfo>		render_info_;
	std::shared_ptr<IRenderCallback>	draw_callback_;
	std::shared_ptr<CSJOutVideoBuf>		out_buf_;

	gl_windowinfo						*window_info_;

	gl_display() {
		render_info_ = nullptr;
		window_info_ = nullptr;
		out_buf_ = nullptr;
	}

	~gl_display() {
		render_info_.reset();
		if (window_info_) {
			//gl_windowinfo_destory(window_info_);
		}
	}
};
