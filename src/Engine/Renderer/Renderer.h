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
#include "Vulkan/VShaders.h"
#include "Vulkan/VImage.h"
#include "Vulkan/VPipeline.h"
#include "Vulkan/VMesh.h"

// datastructures
#include "../Scene/Scene.h"

// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"


VulkanContext InitializeVulkan(GLFWwindow* window);

void DrawFrame(VulkanContext& ctx, GLFWwindow* window, const Scene& scene);

void recreateSwapchainResources(VulkanContext& ctx, GLFWwindow* window);

void waitForFrame(VulkanContext& ctx);

uint32_t getSwapchainImage(VulkanContext& ctx);

void submitFrame(VulkanContext& ctx, uint32_t imageIndex);

void presentFrame(VulkanContext& ctx, uint32_t imageIndex);

void recordCmdBuffers(VulkanContext& ctx, uint32_t imageIndex, const Scene& scene);

void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex);

void setViewport(VulkanContext& ctx, VkCommandBuffer cmd);

void setScissor(VulkanContext& ctx, VkCommandBuffer cmd);

void bindShaders(VulkanContext& ctx, VkCommandBuffer cmd);

void endRendering(VkCommandBuffer cmd);

void DestroyVulkan(VulkanContext& ctx);