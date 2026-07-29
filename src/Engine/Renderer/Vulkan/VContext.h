#pragma once

#include <volk.h>
#include <vector>

enum class SwapchainStatus
{
	Ok,
	NeedsRecreation,
	Invalid
};

struct VulkanGraphicsState
{
	VkPrimitiveTopology topology;

	VkCullModeFlags cullMode;
	VkFrontFace frontFace;

	VkPolygonMode polygonMode;

	bool rasterizerDiscard;

	bool depthTest;
	bool depthBias;
	bool depthWrite;
	bool stencilTest;

	VkSampleCountFlagBits samples;
	VkSampleMask sampleMask;

	bool alphaToCoverage;
	VkBool32 colorBlendEnable;
};


struct SwapchainContext
{

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	SwapchainStatus status;
	uint32_t imageCount = 0;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkImageLayout> imageLayouts;

	VkFormat imageFormat;
	VkExtent2D extent;
	VkPresentModeKHR presentMode;
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
	VulkanGraphicsState currentGraphicsState;


};
