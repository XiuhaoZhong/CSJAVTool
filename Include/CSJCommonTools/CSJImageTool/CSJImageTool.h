#ifndef __CSJIMAGETOOL_H__
#define __CSJIMAGETOOL_H__

#include <windows.h>

#include <string>

#include "CSJCommonToolsDefine.h"

class CSJCOMMONTOOLS_API CSJImageTool {
public:
    CSJImageTool() = default;
    ~CSJImageTool() = default;

    static void SaveToImage(BYTE* imageData);

    /**
     * @brief Read a image in rgb or rgba.
     *
     * @param[in] imagePath     the path of the image file.
     * @param[out] width        save the width of the image.
     * @param[out] height       save the height of the image.
     * @param[out] tride        save the stride of the image.
     * @param[in]  alpha        if true, the image data includes transparent, or not, default is false.
     *
     * @Return  the memory of the image, and user responsible for deleting it.
     */
    static unsigned char *readFromImage(std::string imagePath, int &width, int &height, int &tride, bool alpha = false);
};

#endif