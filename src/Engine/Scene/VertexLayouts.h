#pragma once

#include <glm/glm.hpp>
#include <volk.h>


struct VertexLayout
{
    std::vector<VkVertexInputBindingDescription2EXT> bindings;
    std::vector<VkVertexInputAttributeDescription2EXT> attributes;
};

namespace DefaultVertexLayouts {

    inline VertexLayout Position()
    {
        VertexLayout layout;

        layout.bindings =
        {
            {
                .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
                .binding = 0,
                .stride = sizeof(glm::vec3),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX, 
                .divisor = 1
            }
        };
        layout.attributes =
        {
            {
                .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = 0
            }
        };

        return layout;
    }
    inline VertexLayout PositionColor()
    {
        VertexLayout layout;

        layout.bindings =
        {
            {
                .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
                .binding = 0,
                .stride = sizeof(glm::vec3) + sizeof(glm::vec4),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .divisor = 1
            }
        };

        layout.attributes =
        {
            {
                .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = 0
            },
            {
                .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset = sizeof(glm::vec3)
            }
        };

        return layout;
    }


}