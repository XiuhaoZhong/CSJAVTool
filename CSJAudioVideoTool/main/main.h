#pragma once

#include "base/thread/framework_thread.h"

class MainThread : public nbase::FrameworkThread {
public:
	MainThread() : nbase::FrameworkThread("MainThread") {

	}

	virtual ~MainThread() {}

private:
	/**
	 * 虚函数，初始化主线程
	 *
	 */
	virtual void Init() override;

	/**
	 * 虚函数，主线程退出时，清理工作
	 */
	virtual void Cleanup() override;
};