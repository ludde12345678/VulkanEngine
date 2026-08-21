#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

enum TextureFormat{
    RGBA8,
    RGB8,
    RG8,
    R8
};

TextureData loadPNGTexture(std::string filename){
    TextureData textureData;
    textureData.name = filename;

    int width;
    int height;
    int channels;

    stbi_uc* pixels = stbi_load(
        filename.c_str(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );
    if(!pixels){
        std::cerr << "Failed to load texture: " << filename << ": " << stbi_failure_reason() << std::endl;
        return textureData;
    }
    textureData.width = width;
    textureData.height = height;
    textureData.pixels = std::vector<uint8_t>(pixels, pixels + textureData.width * textureData.height * 4); 
    
     
    stbi_image_free(pixels);
    return textureData;
}