#pragma once
#include <optional>
#include <Types/Components.h>

// object
struct Object
{
    std::string name;
    Components::Transform transform;

    // Optional components
    std::optional<Components::Mesh> mesh;
    std::optional<Components::Camera> camera;
    std::optional<Components::Light> light;
};
