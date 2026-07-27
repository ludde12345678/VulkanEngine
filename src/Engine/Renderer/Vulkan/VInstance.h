#pragma once


#include <glm/glm.hpp>
#include <volk.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "VContext.h"
#include "VConfig.h"
#include "VCallbacks.h"
#include "VDevice.h"
#include "VSwapchain.h"
#include "VCommands.h"
#include "VSync.h"
#include "VUtils.h"

void createInstance(VulkanContext& ctx);

void createSurface(VulkanContext& ctx, GLFWwindow* window);

bool checkExtensionSupport(const std::vector<const char*>& RequestedExtensions);

bool checkValidationLayerSupport(const std::vector<const char*>& RequestedLayers);

void createDebugMessenger(VulkanContext& ctx);

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
