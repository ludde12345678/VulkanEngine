#pragma once

#include <volk.h>
#include <iostream>

#include "VContext.h"
#include "VUtils.h"

void transitionImage(VulkanContext& ctx, VkCommandBuffer cmd, RenderTarget& target, VkImageLayout newLayout);

void transitionSwapchainDepthImage(VulkanContext& ctx, VkCommandBuffer cmd, VkImageLayout oldLayout, VkImageLayout newLayout);

void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

VkImageAspectFlags getAspectFlags(VkFormat format);
