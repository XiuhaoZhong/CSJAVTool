#ifndef __CSJSTRINGTOOL_H__
#define __CSJSTRINGTOOL_H__

#include "CSJCommonToolsDefine.h"

#include <string>

class CSJCOMMONTOOLS_API CSJStringTool {
public:
    CSJStringTool() = default;
    ~CSJStringTool() = default;

    static std::string wstring2string(std::wstring wstr);
    static std::wstring string2wstring(std::string str);

};

#endif