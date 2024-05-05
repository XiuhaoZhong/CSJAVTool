#ifndef __CSJCOMMONTOOL_H__
#define __CSJCOMMONTOOL_H__

#include "CSJCommonToolsDefine.h"

#include <string>

/**
 * Common tool set, including path, file operations and so on.
 */
class CSJCOMMONTOOLS_API CSJCommonTool {
public:
    CSJCommonTool() = default;
    ~CSJCommonTool() = default;

    /**
     * @brief Getting a specified resource path.
     */
    static std::string getResourcePath(std::string relativePath);

    static std::string readFile(const std::string& filePath);

};

#endif // __CSJCOMMONTOOL_H__