#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <volk.h>
#include <iostream>

#include "Vulkan/VContext.h"
#include "Vulkan/VConfig.h"
#include "Vulkan/VCallbacks.h"

#include "Vulkan/VInstance.h"
#include "Vulkan/VDevice.h"
#include "Vulkan/VSwapchain.h"
#include "Vulkan/VCommands.h"
#include "Vulkan/VSync.h"
#include "Vulkan/VImage.h"

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "Uniforms.h"
#include "MeshRenderer.h"

// datastructures
#include "../Scene/Scene.h"
#include "RenderData.h"
// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"


VulkanContext InitializeVulkan(GLFWwindow* window);

void DrawFrame(RendererContext& RenderCtx, GLFWwindow* window, RenderData& Rdata);

void recreateSwapchainResources(VulkanContext& ctx, GLFWwindow* window);

void waitForFrame(VulkanContext& ctx);

uint32_t getSwapchainImage(VulkanContext& ctx);

void submitFrame(VulkanContext& ctx, uint32_t imageIndex);

void presentFrame(VulkanContext& ctx, uint32_t imageIndex);

void recordCmdBuffers(RendererContext& renderCtx, uint32_t imageIndex, const Scene* scene);

void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex);

void setViewport(VulkanContext& ctx, VkCommandBuffer cmd);

void setScissor(VulkanContext& ctx, VkCommandBuffer cmd);

void endRendering(VkCommandBuffer cmd);

void DestroyVulkan(RendererContext& ctx);