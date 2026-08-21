
#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "../Renderer/VertexLayouts.h"



#include <glm/glm.hpp>


struct MeshData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct TextureData
{
    std::string name;

    uint32_t width;
    uint32_t height;

    std::vector<uint8_t> pixels;

};


struct AudioClip
{
    std::string name;

    std::vector<uint8_t> data;

    uint32_t sampleRate;
    uint32_t channels;
};