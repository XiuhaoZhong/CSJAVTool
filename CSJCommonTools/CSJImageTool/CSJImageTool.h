#ifndef __CSJIMAGETOOL_H__
#define __CSJIMAGETOOL_H__

#include "CSJCommonToolsDefine.h"

#include <windows.h>

class CSJCOMMONTOOLS_API CSJImageTool {
public:
    CSJImageTool() = default;
    ~CSJImageTool() = default;

    static void SaveToImage(BYTE* imageData);
};

#endif