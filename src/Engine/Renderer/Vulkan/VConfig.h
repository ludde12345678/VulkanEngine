#pragma once

#include <vulkan/vulkan.h>
#include <filesystem>

namespace VConfig {
	// instance creation
	constexpr bool ENABLE_VALIDATION = true;
	constexpr bool PRINT_DEBUG_INFO = true;

	inline const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};
	inline const std::vector<const char*> instanceExtensions =
	{

	};

	// device

	inline const std::vector<const char*> deviceExtensions = 
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME
	};

	struct DeviceRequirements
	{
		// require
		bool requireGeometryShader = true;
		//prefer
		bool PreferDedicatedCard = true;
	};

	// swapchain
	constexpr uint32_t preferredSwapchainImages = 3;
	constexpr VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	constexpr VkFormat preferredSwapFormat = VK_FORMAT_B8G8R8A8_SRGB;

	// sync
	constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;




	// shaders
	inline std::filesystem::path SHADER_DIR = "shaders";



	
}