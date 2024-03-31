#ifndef __CSJMEDIAUTILS_H__
#define __CSJMEDIAUTILS_H__

#include <stdio.h>

class CSJMediaUtils {
public:
    CSJMediaUtils();
    ~CSJMediaUtils();

    void initWithLogPath(const char* logPath);
    void unInitLog();

    static void logCallback(void *ptr, int level, const char* fmt, va_list vl);

private:
    static FILE *m_logFile;
};

#endif // __CSJMEDIAUTILS_H__
