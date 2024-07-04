#include "CSJImageTool.h"

#include "SOIL2/SOIL2.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stbi/stb_image.h"

void CSJImageTool::SaveToImage(BYTE * imageData) {

}

unsigned char * CSJImageTool::readFromImage(std::string imagePath, int & width, int & height, int & tride, bool alpha) {
    if (imagePath.empty()) {
        return nullptr;
    }

    int withAlpha = alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB;

    unsigned char *image = SOIL_load_image(imagePath.c_str(), &width, &height, &tride, SOIL_LOAD_RGB);
    return image;
}
