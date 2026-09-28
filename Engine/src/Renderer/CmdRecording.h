#pragma once

#include<Types/Scene.h>

#include <volk.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "Vulkan/VContext.h"
#include "RendererConfig.h"
#include "Vulkan/VCallbacks.h"

#include "Vulkan/VInstance.h"
#include "Vulkan/VDevice.h"
#include "Vulkan/VSwapchain.h"
#include "Vulkan/VCommands.h"
#include "Vulkan/VSync.h"
#include "Vulkan/VImage.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "MeshRenderer.h"
#include "Desciptors.h"


// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"




void renderScene( VkCommandBuffer cmd, RendererContext& renderCtx, Scene& scene, ResourceContext& resources);

void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, std::optional <RenderTarget> color, std::optional <RenderTarget> depth);

void setViewport(VulkanContext& ctx, VkCommandBuffer cmd, RenderTarget target);

void setScissor(VulkanContext& ctx, VkCommandBuffer cmd, RenderTarget target);

void endRendering(VkCommandBuffer cmd);