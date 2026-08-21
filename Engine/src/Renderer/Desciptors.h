#pragma once

#include <Types/Scene.h>

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Vulkan/MemoryHandler/MemStructs.h"
#include "Vulkan/MemoryHandler/memory.h"
#include "ShaderData.h"
#include "RendererContext.h"
#include "Vulkan/VUtils.h"
#include "Pipeline.h"
#include "../EngineData.h"

void setupDescriptorResources(RendererContext& ctx, ResourceContext& resources, Scene& scene);

void createDescriptorInfos(RendererContext& ctx);

void createDescriptorLayouts(RendererContext& ctx);

void destroyDescriptorLayouts(RendererContext& ctx);

void createDescriptorPool(RendererContext& ctx);

void allocateDescriptorSets(RendererContext& ctx);

void updateDescriptorSets(RendererContext& ctx);

void destroyDescriptorPool(RendererContext& ctx);

void createUniformBuffers(RendererContext& ctx);

void destroyUniformBuffers(RendererContext& ctx);

void bindDescriptorSets(RendererContext& ctx, VkCommandBuffer cmd);

void updatePerFrameUniformBuffers(RendererContext& RenderCtx, RenderData& Rdata);
