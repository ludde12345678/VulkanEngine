#pragma once

#include <volk.h>
#include <iostream>

#include "VContext.h"
#include "VUtils.h"


void createCommandPool(VulkanContext& ctx);
void createCommandBuffers(VulkanContext& ctx);
VkCommandBuffer beginSingleTimeCommands(VulkanContext& ctx);
void endSingleTimeCommands(VulkanContext& ctx, VkCommandBuffer cmd);
void beginCommandBuffer(VkCommandBuffer cmd, VkCommandBufferUsageFlags flags = 0);
void endCommandBuffer(VkCommandBuffer cmd);
void destroyCommandPool(VulkanContext& ctx);