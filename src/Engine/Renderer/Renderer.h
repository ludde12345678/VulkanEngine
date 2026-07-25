#pragma once

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <iostream>

#include "Vulkan/VContext.h"
#include "Vulkan/VConfig.h"
#include "Vulkan/VCallbacks.h"

#include "Vulkan/VInstance.h"
#include "Vulkan/VDevice.h"
#include "Vulkan/VSwapchain.h"
#include "Vulkan/VCommands.h"
#include "Vulkan/VSync.h"
#include "Vulkan/shaders.h"



VulkanContext InitializeVulkan(GLFWwindow* window);

void DrawFrame(VulkanContext& ctx, GLFWwindow* window);

void waitForFrame(VulkanContext& ctx);

uint32_t getSwapchainImage(VulkanContext& ctx);

void submitFrame(VulkanContext& ctx, uint32_t imageIndex);

void presentFrame(VulkanContext& ctx, uint32_t imageIndex);

void recordCmdBuffers(VulkanContext& ctx, uint32_t imageIndex);

void beginCmdBuffer(VkCommandBuffer cmd);

void transitionSwapchainImage(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex, VkImageLayout newLayout);

void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

VkImageAspectFlags getAspectFlags(VkFormat format);

void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex);

void setViewport(VulkanContext& ctx, VkCommandBuffer cmd);

void setScissor(VulkanContext& ctx, VkCommandBuffer cmd);

void bindShaders(VulkanContext& ctx, VkCommandBuffer cmd);

void endRendering(VkCommandBuffer cmd);

void endCommandBuffer(VkCommandBuffer cmd);

void DestroyVulkan(VulkanContext& ctx);