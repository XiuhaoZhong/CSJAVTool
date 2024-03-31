#include "CSJMediaLog.h"

#include "CSJMediaPegHeader.h"

FILE* CSJMediaUtils::m_logFile = nullptr;

CSJMediaUtils::CSJMediaUtils() {
    m_logFile = nullptr;
}

CSJMediaUtils::~CSJMediaUtils() {
    unInitLog();
}

void CSJMediaUtils::initWithLogPath(const char * logPath) {
    if (!logPath) {
        return ;
    }

    fopen(logPath, "w+");

    av_log_set_level(AV_LOG_TRACE); //设置日志级别
    av_log_set_callback(CSJMediaUtils::logCallback);
}

void CSJMediaUtils::unInitLog() {
    av_log_set_callback(NULL);
    if (m_logFile) {
        fflush(m_logFile);
        fclose(m_logFile);
        m_logFile = nullptr;
    }
}

void CSJMediaUtils::logCallback(void * ptr, int level, const char * fmt, va_list vl) {
    if (!m_logFile) {
        return;
    }

    vfprintf(m_logFile, fmt, vl);
    fflush(m_logFile);
}
