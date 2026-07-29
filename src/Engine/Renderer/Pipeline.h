#pragma once

#include <volk.h>
#include <iostream>

#include "Vulkan/VContext.h"
#include "Vulkan/VUtils.h"

#include "RenderData.h";
#include "RendererContext.h"

void setupDynamicState(VulkanContext& ctx, VkCommandBuffer cmd);

void createPipelineLayout(RendererContext& ctx);

void destroyPipelineLayout(RendererContext& ctx);