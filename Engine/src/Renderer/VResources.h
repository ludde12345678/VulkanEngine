#pragma once

#include "Vulkan/MemoryHandler/MemStructs.h"
#include "VertexLayouts.h"

struct Mesh
{

    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t normalsCount = 0;
    uint32_t textCoordsCount = 0;


    VkIndexType indexType = VK_INDEX_TYPE_UINT32;
    VertexLayout layout;
};
struct Texture
{
    AllocatedImage image;
    VkSampler sampler = VK_NULL_HANDLE;
};


