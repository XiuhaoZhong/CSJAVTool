#include "CSJStringTool.h"

#include <windows.h>

std::string CSJStringTool::wstring2string(std::wstring wstr) {
    std::string res;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    res.append(buffer);
    delete[] buffer;
    return res;
}

std::wstring CSJStringTool::string2wstring(std::string str) {
    if (str.size() == 0) {
        return NULL;
    }

    std::wstring wstr;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    WCHAR *buffer = new WCHAR[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, len);
    
    buffer[len] = '\0';
    wstr.append(buffer);

    delete[] buffer;
    return wstr;
}
