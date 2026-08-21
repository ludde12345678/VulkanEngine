#pragma once
#include <string>

struct MeshHandle
{
    std::string name = "";
    uint32_t meshIndex = 0;
    bool isValid = false;
};

struct TextureHandle
{
    std::string name = "";
    uint32_t textureIndex = 0;
    bool isValid = false;
};