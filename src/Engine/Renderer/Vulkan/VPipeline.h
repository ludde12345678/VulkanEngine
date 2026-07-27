#pragma once

#include <volk.h>
#include <iostream>

#include "VContext.h"
#include "VUtils.h"

void setupDynamicState(VulkanContext& ctx, VkCommandBuffer cmd);
