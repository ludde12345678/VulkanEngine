#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#include "VContext.h"


void createCommandPool(VulkanContext& ctx);
void createCommandBuffers(VulkanContext& ctx);
void destroyCommandPool(VulkanContext& ctx);