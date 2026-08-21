#pragma once
#include <Types/Handles.h>
#include <glm/glm.hpp>
enum class ObjectComponents
{
    Mesh,
    Material,
    PointLight,
    Camera,
};

struct Material
{
    TextureHandle AlbedoIndex = TextureHandle{};
    TextureHandle NormalIndex = TextureHandle{};
    TextureHandle RoughnessIndex = TextureHandle{};
    glm::vec4 color = glm::vec4(1.0f);
    float specularIntensity = 10.0f;
};

struct Object
{
    std::string name;
    MeshHandle mesh;
    Material material;

    std::vector<ObjectComponents> components;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};