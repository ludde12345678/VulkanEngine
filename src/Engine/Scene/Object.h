#pragma once

#include "../Renderer/Vulkan/MemoryHandler/MemStructs.h"
#include "VertexLayouts.h"

// engine structs

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
    VkSampler sampler;
};

// engine agnostic structs

enum class ObjectComponents
{
    Mesh,
    Material,
    PointLight,
    Camera,
};

struct Material
{
    int32_t AlbedoIndex;
    int32_t NormalIndex;
    int32_t RoughnessIndex;
    glm::vec4 color = glm::vec4(1.0f);
    float specularIntensity = 1.0f;
};
struct Object
{
    std::string name;
    int32_t meshIndex;
    int32_t materialIndex;

    std::vector<ObjectComponents> components;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};
