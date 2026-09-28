#pragma once
#include <volk.h>
#include <optional>

struct RenderStageImageConfig {
	VkImageCreateInfo image{};
	VkImageViewCreateInfo view{};
	std::optional<VkSamplerCreateInfo> sampler;
};


namespace RenderStageResources {

	// shadow map
    RenderStageImageConfig shadowMapConfig{
        .image = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_D32_SFLOAT,
            .extent = { 16384, 16384, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage =
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        },

        .view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_D32_SFLOAT,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        },

        .sampler = VkSamplerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .compareEnable = VK_TRUE,
            .compareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            
        }
    };
}