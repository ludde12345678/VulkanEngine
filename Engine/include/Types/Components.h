#pragma once
#include <optional>
#include <glm/glm.hpp>
#include <Types/Handles.h>

#include <algorithm>
#include <cctype>
#include <ranges>
#include <stdexcept>


namespace Components {

    struct Transform {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };
    struct Mesh {
        MeshHandle mesh;
        std::optional<MaterialHandle> material;
    };
    enum class CameraProjection
    {
        Perspective,
        Orthographic
    };
    inline CameraProjection CameraProjectionFromString(const std::string& str)
    {
        std::string value = str;
        std::ranges::transform(value, value.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        if (value == "perspective")
            return CameraProjection::Perspective;

        if (value == "orthographic")
            return CameraProjection::Orthographic;

        throw std::runtime_error("Unknown Camera projection type: " + str);
    }


    struct Camera {
        CameraProjection projection = CameraProjection::Perspective;
        float aspectRatio = 16.0f / 9.0f;
        float fov = 80.0f;

        float nearPlane = 0.1f;
        float farPlane = 1000.0f;
    };


    enum class LightType : uint32_t
    {
        Point = 0,
        Directional = 1,
        Spot = 2

        
    };
    inline constexpr uint32_t operator+(LightType c) noexcept {
        return static_cast<uint32_t>(c);
    }
    inline LightType lightTypeFromString(const std::string& str)
    {
        std::string value = str;
        std::ranges::transform(value, value.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        if (value == "point")
            return LightType::Point;

        if (value == "directional")
            return LightType::Directional;

        if (value == "spot")
            return LightType::Spot;

        throw std::runtime_error("Unknown light type: " + str);
    };

    struct Light {
        LightType type = LightType::Point;
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
        float range = 0;
        float spotInnerAngle = 0;
        float spotOuterAngle = 0;
        bool castsShadows = false;
        float shadowDistance = 0;
    };


}