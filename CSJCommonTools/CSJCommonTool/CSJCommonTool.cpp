#include "CSJCommonTool.h"

#include <Windows.h>
#include <shlwapi.h>

#include <fstream>
#include <iostream>

#include "CSJStringTool/CSJStringTool.h"

using std::string;
using std::ifstream;

std::string CSJCommonTool::getResourcePath(std::string relativePath) {
    TCHAR curPath[255];
    DWORD pathLen = ::GetCurrentDirectory(255, curPath);
    if (pathLen > 0) {

    }

    std::wstring tmpPath(curPath);

    std::string resPath = CSJStringTool::wstring2string(tmpPath);
    resPath.append("\\" + relativePath);
 
    return resPath;
}

std::string CSJCommonTool::readFile(const std::string & filePath) {
    string content;

    ifstream fileStream(filePath, std::ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}
