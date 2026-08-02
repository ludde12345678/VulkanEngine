#pragma once

#include <volk.h>
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
	inline std::vector<VkFormat> preferredDepthFormats =
	{
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT
	};

	// sync
	constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;




	// shaders
	inline std::filesystem::path SHADER_DIR = "shaders";

	// default Renderstate

	inline VulkanGraphicsState DefaultGraphicsState
	{
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.rasterizerDiscard = false,
		.depthTest = true,
		.compareOp = VK_COMPARE_OP_LESS,
		.depthBias = false,
		.depthWrite = true,
		.stencilTest = false,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.sampleMask = 0xFFFFFFFF,


		.alphaToCoverage = false,
		.colorBlendEnable = VK_FALSE
	};

	
}