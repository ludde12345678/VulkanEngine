#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <volk.h>
#include <iostream>
#include <optional>
#include <set>
#include "VContext.h"
#include "VConfig.h"
#include "VUtils.h"


struct QueueFamilies {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> transferFamily;

	bool hasRequiredQueues() {
		// add queue requirements here
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

void pickPhysicalDevice(VulkanContext& ctx);

void createLogicalDevice(VulkanContext& ctx);

bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

bool checkDeviceFeatures(VkPhysicalDevice device);

bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& RequestedExtensions);

QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

void logPhysicalDevice(VkPhysicalDevice device);

void logLogicalDevice(VulkanContext& ctx);
