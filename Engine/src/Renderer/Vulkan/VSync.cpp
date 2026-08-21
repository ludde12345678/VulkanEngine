#include "VSync.h"

void initializeSync(VulkanContext& ctx) {
	SyncContext sync{};
	sync.imageAvailableSemaphores.resize(VConfig::MAX_FRAMES_IN_FLIGHT);
	sync.renderFinishedSemaphores.resize(ctx.swapchainContext.imageCount);
	sync.inFlightFences.resize(VConfig::MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < VConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(ctx.device, &semaphoreCreateInfo, nullptr, &sync.imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(ctx.device, &fenceCreateInfo, nullptr, &sync.inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create synchronization objects.");
		}
	}
	for (uint32_t i = 0; i < ctx.swapchainContext.imageCount; i++)
	{
		if (vkCreateSemaphore(ctx.device, &semaphoreCreateInfo, nullptr, &sync.renderFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create synchronization objects(RenderFinishedSemaphores).");
		}
	}
	ctx.syncContext = sync;

}


void destroySync(VulkanContext& ctx) {
	vkDeviceWaitIdle(ctx.device);
	for (uint32_t i = 0; i < VConfig::MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(ctx.device, ctx.syncContext.imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(ctx.device, ctx.syncContext.inFlightFences[i], nullptr);
	}
	for (uint32_t i = 0; i < ctx.swapchainContext.imageCount; i++)
	{
		vkDestroySemaphore(ctx.device, ctx.syncContext.renderFinishedSemaphores[i], nullptr);
	}

}

void recreateImageSync(VulkanContext& ctx) {
	for (VkSemaphore semaphore : ctx.syncContext.renderFinishedSemaphores)
	{
		vkDestroySemaphore(ctx.device, semaphore, nullptr);
	}
	ctx.syncContext.renderFinishedSemaphores.resize(ctx.swapchainContext.imageCount);
	
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	for (uint32_t i = 0; i < ctx.swapchainContext.imageCount; i++)
	{
		if (vkCreateSemaphore(ctx.device, &semaphoreCreateInfo, nullptr, &ctx.syncContext.renderFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create synchronization objects(RenderFinishedSemaphores).");
		}
	}
}