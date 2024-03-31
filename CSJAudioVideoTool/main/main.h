#pragma once

#include "base/thread/framework_thread.h"

class MainThread : public nbase::FrameworkThread {
public:
	MainThread() : nbase::FrameworkThread("MainThread") {

	}

	virtual ~MainThread() {}

private:
	/**
	 * �麯������ʼ�����߳�
	 *
	 */
	virtual void Init() override;

	/**
	 * �麯�������߳��˳�ʱ��������
	 */
	virtual void Cleanup() override;
};