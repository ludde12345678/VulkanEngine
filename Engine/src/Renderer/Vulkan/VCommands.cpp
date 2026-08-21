#include "VCommands.h"

void createCommandPool(VulkanContext& ctx) {
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = ctx.queueContext.graphicsQueueFamily;


	VkCommandPool commandPool = VK_NULL_HANDLE;
	if (vkCreateCommandPool(ctx.device, &cmdPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Command Pool Creation failed");
	}
	ctx.cmdPool = commandPool;

}
void createCommandBuffers(VulkanContext& ctx) {
	const uint32_t commandBufferCount = static_cast<uint32_t>(ctx.swapchainContext.images.size());
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = ctx.cmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = commandBufferCount;
	
	
	ctx.cmdBuffers.resize(commandBufferCount);
	if (vkAllocateCommandBuffers(ctx.device, &allocInfo, ctx.cmdBuffers.data()) != VK_SUCCESS) { throw std::runtime_error("Command buffers could not be allocated"); }

}
VkCommandBuffer beginSingleTimeCommands(VulkanContext& ctx)
{
	const uint32_t commandBufferCount = 1;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = ctx.cmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = commandBufferCount;
	
	VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
	if (vkAllocateCommandBuffers(ctx.device, &allocInfo, &cmdBuffer) != VK_SUCCESS) { throw std::runtime_error("Command buffers could not be allocated"); }
	beginCommandBuffer(cmdBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	return cmdBuffer;
}
void endSingleTimeCommands(VulkanContext& ctx, VkCommandBuffer cmd)
{
	// end command buffer
	endCommandBuffer(cmd);

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	vkCreateFence(ctx.device, &fenceInfo, nullptr, &fence);


	// submit commandbuffer
	VkCommandBufferSubmitInfo cmdSubmitInfo{};
	cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdSubmitInfo.commandBuffer = cmd;
	cmdSubmitInfo.deviceMask = 0;

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;

	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmdSubmitInfo;

	vkQueueSubmit2(ctx.queueContext.graphicsQueue, 1, &submitInfo, fence);

	vkWaitForFences(ctx.device, 1, &fence, VK_TRUE, UINT64_MAX);

	vkDestroyFence(ctx.device, fence, nullptr);

	vkFreeCommandBuffers(ctx.device, ctx.cmdPool, 1, &cmd);


}
void beginCommandBuffer(VkCommandBuffer cmd, VkCommandBufferUsageFlags flags) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags;

	if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) { throw std::runtime_error("BeginCommandBuffer failed"); }
	
}


void endCommandBuffer(VkCommandBuffer cmd) {
	if (vkEndCommandBuffer(cmd) != VK_SUCCESS) { throw std::runtime_error("EndCommandBuffer failed"); }
}


void destroyCommandPool(VulkanContext& ctx) {
	vkDeviceWaitIdle(ctx.device);
	if (ctx.cmdPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(ctx.device, ctx.cmdPool, nullptr);
		ctx.cmdPool = VK_NULL_HANDLE;
	}

}