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




void recordCmdBuffer(RendererContext& renderCtx, ResourceContext& resources, uint32_t imageIndex, Scene* scene);

void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex);

void setViewport(VulkanContext& ctx, VkCommandBuffer cmd);

void setScissor(VulkanContext& ctx, VkCommandBuffer cmd);

void endRendering(VkCommandBuffer cmd);