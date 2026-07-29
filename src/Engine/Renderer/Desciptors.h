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
#include "Vulkan/VUtils.h"

void createDescriptorLayouts(RendererContext& ctx);

void destroyDescriptorLayouts(RendererContext& ctx);

void createDescriptorPool(RendererContext& ctx);

void allocateDescriptorSets(RendererContext& ctx);

void updateDescriptorSets(RendererContext& ctx);

void destroyDescriptorPool(RendererContext& ctx);
