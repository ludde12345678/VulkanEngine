#pragma once



#include <volk.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "MeshRenderer.h"
#include "Desciptors.h"

// datastructures
#include "../Scene/Scene.h"
#include "../App/EngineData.h"
// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"


void InitializeVulkan(RendererContext& RenderCtx, GLFWwindow* window);

void DrawFrame(RendererContext& RenderCtx, GLFWwindow* window, Scene& scene, RenderData& Rdata);

void updateAllUniformBuffers(RendererContext& RenderCtx, RenderData& Rdata);

void recreateSwapchainResources(VulkanContext& ctx, GLFWwindow* window);

void waitForFrame(VulkanContext& ctx);

uint32_t getSwapchainImage(VulkanContext& ctx);

void submitFrame(VulkanContext& ctx, uint32_t imageIndex);

void presentFrame(VulkanContext& ctx, uint32_t imageIndex);

void recordCmdBuffers(RendererContext& renderCtx, uint32_t imageIndex, Scene* scene);

void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex);

void setViewport(VulkanContext& ctx, VkCommandBuffer cmd);

void setScissor(VulkanContext& ctx, VkCommandBuffer cmd);

void endRendering(VkCommandBuffer cmd);

void DestroyVulkan(RendererContext& ctx, Scene scene);

void printRendererDebugInfo(RendererContext& ctx, RenderData& renderData, Scene& scene);
