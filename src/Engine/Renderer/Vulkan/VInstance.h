#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <iostream>

#include "VContext.h"
#include "VConfig.h"
#include "VCallbacks.h"
#include "VDevice.h"
#include "VSwapchain.h"
#include "VCommands.h"
#include "VSync.h"

void createInstance(VulkanContext& ctx);

void createSurface(VulkanContext& ctx, GLFWwindow* window);

bool checkExtensionSupport(const std::vector<const char*>& RequestedExtensions);

bool checkValidationLayerSupport(const std::vector<const char*>& RequestedLayers);

void createDebugMessenger(VulkanContext& ctx);

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT DebugMessenger, const VkAllocationCallbacks* Allocator);
