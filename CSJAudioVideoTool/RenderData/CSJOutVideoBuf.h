#pragma once

#include <windows.h>
#include <vector>
#include <mutex>
#include <atomic>

using BufferArray = std::vector<uint8_t>;

struct CSJOutVideoData {
	BufferArray video_data_;
};

// ����
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
		// TODO: �˴����erase������ʲô��˼��
		// ����һ������ȡ��֮�󣬴�vector��ɾ��;
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

	// ����׼���õ�֪ͨ;
	std::function<void()> notify_;
};

// ������ƵĻ�����Ϣ;
class IRenderInfo {
public:
	virtual HWND GetHwnd() = 0; // ����Ҫ���ƵĴ��ھ��;

	std::atomic<bool> b_enable = true; // �Ƿ���Ҫ����;
	std::atomic<bool> b_remove = false; // �Ƿ���Ҫ�Ƴ���ǰ����;

	std::atomic<bool> source_updated = false;
	std::atomic<bool> source_preview = false;

	bool b_draw_outlines = true;
};

// ���ƽӿ�;
class IRenderCallback {
public:
	virtual bool Draw(std::shared_ptr<IRenderInfo> info, std::shared_ptr<CSJOutVideoBuf> buf) = 0;
};

