#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Vulkan/MemoryHandler/MemStructs.h"
#include "Vulkan/MemoryHandler/memory.h"
#include "ShaderData.h"
#include "RenderData.h"
#include "RendererContext.h"


UniformBuffers AllocateUniformBuffers(VulkanContext& ctx);

void bindUniformBuffers(RendererContext& ctx, VkCommandBuffer cmd);


