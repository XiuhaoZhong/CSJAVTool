#pragma once

#include <windows.h>
#include <vector>
#include <mutex>
#include <atomic>

using BufferArray = std::vector<uint8_t>;

struct CSJOutVideoData {
	BufferArray video_data_;
};

// 保存
class CSJOutVideoBuf {
public:
	CSJOutVideoBuf() {
		notify_ = nullptr;
	}

	~CSJOutVideoBuf() {}

	void Initial(int buf_num, int buf_size, std::function<void()> notify) {
		for (int i = 0; i < buf_num; i++) {
			std::shared_ptr<CSJOutVideoData> out_buf = std::make_shared<CSJOutVideoData>();
			out_buf->video_data_.reserve(buf_size);
			write_buf_.push_back(out_buf);
		}

		notify_ = notify;
	}

	std::shared_ptr<CSJOutVideoData> get_write_buf() {
		std::lock_guard<std::recursive_mutex> lock(write_mutex_);

		std::shared_ptr<CSJOutVideoData> video_buf = nullptr;
		// TODO: 此处这个erase操作是什么意思？
		// 将第一个数据取出之后，从vector中删除;
		auto iter = write_buf_.begin();
		if (iter != write_buf_.end()) {
			video_buf = *iter;
			write_buf_.erase(iter);
		}

		return video_buf;
	}

	void set_read_buf(std::shared_ptr<CSJOutVideoData> buf) {
		std::lock_guard<std::recursive_mutex> lock(read_mutex_);
		read_buf_.push_back(buf);

		if (notify_) {
			notify_();
		}
	}

	void set_write_buf(std::shared_ptr<CSJOutVideoData> buf) {
		std::lock_guard<std::recursive_mutex> lock(write_mutex_);
		write_buf_.push_back(buf);
	}

	std::shared_ptr<CSJOutVideoData> get_read_buf() {
		std::lock_guard<std::recursive_mutex> lock(read_mutex_);

		std::shared_ptr<CSJOutVideoData> video_buf = nullptr;
		auto iter = read_buf_.begin();
		if (iter != read_buf_.end()) {
			video_buf = *iter;
			read_buf_.erase(iter);
		}

		return video_buf;
	}

protected:
	std::recursive_mutex write_mutex_;
	std::recursive_mutex read_mutex_;

	std::vector<std::shared_ptr<CSJOutVideoData>> write_buf_;
	std::vector<std::shared_ptr<CSJOutVideoData>> read_buf_;

	// 数据准备好的通知;
	std::function<void()> notify_;
};

// 保存绘制的基本信息;
class IRenderInfo {
public:
	virtual HWND GetHwnd() = 0; // 返回要绘制的窗口句柄;

	std::atomic<bool> b_enable = true; // 是否需要绘制;
	std::atomic<bool> b_remove = false; // 是否需要移除当前绘制;

	std::atomic<bool> source_updated = false;
	std::atomic<bool> source_preview = false;

	bool b_draw_outlines = true;
};

// 绘制接口;
class IRenderCallback {
public:
	virtual bool Draw(std::shared_ptr<IRenderInfo> info, std::shared_ptr<CSJOutVideoBuf> buf) = 0;
};

