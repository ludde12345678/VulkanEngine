#pragma once

#include "../Renderer/Vulkan/MemoryHandler/MemStructs.h"
#include "VertexLayouts.h"

struct Mesh
{
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    VertexLayout layout;
};
