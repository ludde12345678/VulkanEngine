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
void destroyCommandPool(VulkanContext& ctx) {
	if (ctx.cmdPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(ctx.device, ctx.cmdPool, nullptr);
		ctx.cmdPool = VK_NULL_HANDLE;
	}

}