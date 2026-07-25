#pragma once

#include <vulkan/vulkan.h>
#include <vector>

enum class SwapchainStatus
{
	Ok,
	NeedsRecreation,
	Invalid
};

struct PipelineContext
{
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};

struct ShaderContext
{
	VkShaderEXT vertexShader = VK_NULL_HANDLE;
	VkShaderEXT fragmentShader = VK_NULL_HANDLE;
};

struct SwapchainContext
{

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	SwapchainStatus status;
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
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

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
	ShaderContext shaderContext;
	PipelineContext pipelineContext;


};

template<typename FunctionType>
FunctionType loadInstanceFunction(VkInstance instance, const char name[]) {
	FunctionType func = reinterpret_cast<FunctionType>(vkGetInstanceProcAddr(instance, name));
	if (func == nullptr) { throw std::runtime_error("Instance extension function not found"); }
	return func;
}
template<typename FunctionType>
FunctionType loadDeviceFunction(VkDevice device, const char name[]) {
	FunctionType func = reinterpret_cast<FunctionType>(vkGetDeviceProcAddr(device, name));
	if (func == nullptr) { throw std::runtime_error("Device extension function not found"); }
	return func;
}