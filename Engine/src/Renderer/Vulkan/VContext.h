#pragma once

#include <volk.h>
#include <vector>
#include "MemoryHandler/MemStructs.h"

enum class SwapchainStatus
{
	Ok,
	NeedsRecreation,
	Invalid
};
struct VulkanDepthState
{
	VkBool32 testEnable = VK_FALSE;
	VkBool32 writeEnable = VK_FALSE;
	VkCompareOp compareOp = VK_COMPARE_OP_EQUAL;

	VkBool32 biasEnable = VK_FALSE;

	VkDepthBiasInfoEXT biasInfo{
		.sType = VK_STRUCTURE_TYPE_DEPTH_BIAS_INFO_EXT,
		.depthBiasConstantFactor = 1.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 1.0f
	};
};

struct VulkanStencilState
{
	VkBool32 testEnable = VK_FALSE;

	VkStencilOpState front{};
	VkStencilOpState back{};
};
struct VulkanGraphicsState
{
	// assembly
	VkPrimitiveTopology topology;
	VkBool32 primitiveRestart;
	float lineWidth;
	// rasterisation
	VkCullModeFlags cullMode;
	VkFrontFace frontFace;
	VkPolygonMode polygonMode;
	VkBool32 rasterizerDiscard;

	// depth
	VulkanDepthState depth{};
	
	// stencil
	VulkanStencilState stencil{};
	
	//MSAA
	VkSampleCountFlagBits samples;
	VkSampleMask sampleMask;
	VkBool32 alphaToCoverage;


	// color blending
	std::vector<VkBool32> colorBlendEnables;
	std::vector<VkColorBlendEquationEXT> blendEquations;
	std::vector<VkColorComponentFlags> colorWriteMasks;

};
struct RenderTarget {
	VkImage image;
	VkImageView imageView;

	VkFormat format = VK_FORMAT_UNDEFINED;
	VkExtent2D extent{};
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

};


struct SwapchainContext
{

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	SwapchainStatus status = SwapchainStatus::Ok;
	uint32_t imageCount = 0;

	std::vector<RenderTarget> colorTargets;
	std::vector<RenderTarget> depthTargets;

	std::vector<AllocatedImage> allocatedDepthImages;

	VkPresentModeKHR presentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
};

struct QueueContext
{
	VkQueue graphicsQueue;
	uint32_t graphicsQueueFamily;
	VkQueue presentQueue;
	uint32_t presentQueueFamily;
};
struct SyncContext
{
	// frame based
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkFence> inFlightFences;
	// image based
	std::vector<VkSemaphore> renderFinishedSemaphores;

	size_t currentFrame = 0;
};

struct VulkanContext
{
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	QueueContext queueContext;
	SwapchainContext swapchainContext;
	VkCommandPool cmdPool;
	std::vector<VkCommandBuffer> cmdBuffers;
	SyncContext syncContext;


};
