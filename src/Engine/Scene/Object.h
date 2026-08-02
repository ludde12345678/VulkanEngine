#pragma once

#include "../Renderer/Vulkan/MemoryHandler/MemStructs.h"
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


struct Object
{
    uint32_t meshIndex;
    glm::mat4 transform = glm::mat4(1.0f);
};
