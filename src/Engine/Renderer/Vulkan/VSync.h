#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#include "VContext.h"
#include "VConfig.h"


void initializeSync(VulkanContext& ctx);

void destroySync(VulkanContext& ctx);
