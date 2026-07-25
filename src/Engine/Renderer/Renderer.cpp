#include "Renderer.h"


VulkanContext InitializeVulkan(GLFWwindow* window) {
	VulkanContext ctx{};

	createInstance(ctx);
	createDebugMessenger(ctx);
	createSurface(ctx, window);
	pickPhysicalDevice(ctx);
	createLogicalDevice(ctx);
	createSwapChain(ctx, window);
	createCommandPool(ctx);
	createCommandBuffers(ctx);
	createPipelineLayout(ctx);
	createShaders(ctx);
	initializeSync(ctx);

	return ctx;
}

void DestroyVulkan(VulkanContext& ctx) {
	destroySync(ctx);
	destroyShaders(ctx);
	destroyPipelineLayout(ctx);
	destroyCommandPool(ctx);
	destroySwapchain(ctx);
	vkDestroyDevice(ctx.device, nullptr);
	vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);

	if (ctx.debugMessenger != VK_NULL_HANDLE)
	{
		DestroyDebugUtilsMessengerEXT(
			ctx.instance,
			ctx.debugMessenger,
			nullptr
		);
	}

	vkDestroyInstance(ctx.instance, nullptr);
	std::cout << "Vulkan successfully destroyed";
}

void DrawFrame(VulkanContext &ctx, GLFWwindow* window) {

	waitForFrame(ctx);
	uint32_t imageIndex = getSwapchainImage(ctx);
	if (ctx.swapchainContext.status == SwapchainStatus::Invalid)
	{
		recreateSwapchain(ctx, window);
		return;
	}
	vkResetFences(ctx.device, 1, &ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame]);
	recordCmdBuffers(ctx, imageIndex);
	submitFrame(ctx, imageIndex);
	presentFrame(ctx, imageIndex);
	ctx.syncContext.currentFrame = (ctx.syncContext.currentFrame + 1) % VConfig::MAX_FRAMES_IN_FLIGHT;

}

void waitForFrame(VulkanContext& ctx) {

	vkWaitForFences(ctx.device, 1, &ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame], VK_TRUE, UINT64_MAX);
}
uint32_t getSwapchainImage(VulkanContext& ctx) {

	VkSemaphore imageAvailable = ctx.syncContext.imageAvailableSemaphores[ctx.syncContext.currentFrame];
	uint32_t imageIndex = 0;

	VkResult res = vkAcquireNextImageKHR(ctx.device, ctx.swapchainContext.swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

	if(res == VK_SUCCESS) { return imageIndex; }
	else if (res == VK_SUBOPTIMAL_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Suboptimal Swapchain"; }
		ctx.swapchainContext.status = SwapchainStatus::NeedsRecreation;
		return imageIndex;
	}
	else if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		if(VConfig::PRINT_DEBUG_INFO){ std::cout << "WARNING: Invalid Swapchain"; }
		ctx.swapchainContext.status = SwapchainStatus::Invalid;
		return UINT32_MAX;
	}
	else {
		throw std::runtime_error("Error: Unknown error in acquire swapchain image");
	}

}


void submitFrame(VulkanContext& ctx, uint32_t imageIndex) {
	
	// wait for frame to become available
	VkSemaphoreSubmitInfo waitSemInfo{};
	waitSemInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waitSemInfo.semaphore = ctx.syncContext.imageAvailableSemaphores[ctx.syncContext.currentFrame];
	waitSemInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	waitSemInfo.deviceIndex = 0;

	// submit commandbuffer
	VkCommandBufferSubmitInfo cmdSubmitInfo{};
	cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdSubmitInfo.commandBuffer = ctx.cmdBuffers[imageIndex];
	cmdSubmitInfo.deviceMask = 0;

	// when reder is done, singal renderfinished semaphore
	VkSemaphoreSubmitInfo signalSemInfo{};
	signalSemInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signalSemInfo.semaphore = ctx.syncContext.renderFinishedSemaphores[ctx.syncContext.currentFrame];
	signalSemInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	signalSemInfo.deviceIndex = 0;

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;

	submitInfo.waitSemaphoreInfoCount = 1;
	submitInfo.pWaitSemaphoreInfos = &waitSemInfo;

	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmdSubmitInfo;

	submitInfo.signalSemaphoreInfoCount = 1;
	submitInfo.pSignalSemaphoreInfos = &signalSemInfo;


	// submit work, when finished signal infligt fence, so cpu know this frameslot is cleared for reuse
	vkQueueSubmit2(ctx.queueContext.graphicsQueue, 1, &submitInfo, ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame]);


}
void presentFrame(VulkanContext& ctx, uint32_t imageIndex) {
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	// wait for the renderFinished semaphore to signal, aka wait for image to finish rendering
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &ctx.syncContext.renderFinishedSemaphores[ctx.syncContext.currentFrame];

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &ctx.swapchainContext.swapchain;

	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkResult res = vkQueuePresentKHR(ctx.queueContext.presentQueue, &presentInfo);

	if (res == VK_SUCCESS) { return; }
	else if (res == VK_SUBOPTIMAL_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Suboptimal Swapchain"; }
		ctx.swapchainContext.status = SwapchainStatus::NeedsRecreation;
		return;
	}
	else if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Invalid Swapchain"; }
		ctx.swapchainContext.status = SwapchainStatus::Invalid;
		return;
	}
	else {
		throw std::runtime_error("Error: Unknown error in acquire swapchain image");
	}

}





void recordCmdBuffers(VulkanContext& ctx, uint32_t imageIndex) {
	VkCommandBuffer cmd = ctx.cmdBuffers[imageIndex];
	beginCmdBuffer(cmd);
	transitionSwapchainImage(ctx, cmd, imageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	beginRendering(ctx, cmd, imageIndex);
	setViewport(ctx, cmd);
	setScissor(ctx, cmd);
	bindShaders(ctx, cmd);

	// draw here

	auto CmdSetPrimitiveTopologyEXT = loadDeviceFunction<PFN_vkCmdSetPrimitiveTopologyEXT>(ctx.device, "vkCmdSetPrimitiveTopologyEXT");
	CmdSetPrimitiveTopologyEXT(
		cmd,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
	);

	vkCmdDraw(cmd, 3, 1, 0, 0);


	endRendering(cmd);
	transitionSwapchainImage(ctx, cmd, imageIndex, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	endCommandBuffer(cmd);


}

void beginCmdBuffer(VkCommandBuffer cmd) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) { throw std::runtime_error("BeginCommandBuffer failed"); }

}
void transitionSwapchainImage(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex, VkImageLayout newLayout) {
	VkImageLayout oldLayout = ctx.swapchainContext.imageLayouts[imageIndex];
	transitionImageLayout(
		cmd,
		ctx.swapchainContext.images[imageIndex],
		ctx.swapchainContext.imageFormat,
		oldLayout,
		newLayout
	);
	ctx.swapchainContext.imageLayouts[imageIndex] =	newLayout;

}

void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	
	VkImageSubresourceRange resourceRange{};
	resourceRange.aspectMask = getAspectFlags(format);

	resourceRange.baseMipLevel = 0;
	resourceRange.levelCount = 1;

	resourceRange.baseArrayLayer = 0;
	resourceRange.layerCount = 1;

	
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = nullptr;
	barrier.image = image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.subresourceRange = resourceRange;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// if ladder from hell
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.dstAccessMask = VK_ACCESS_2_NONE;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
			VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}


	else 
	{
		throw std::runtime_error("Unsupported image transition");
	}

	// check if they are set
	/*
	if (barrier.dstStageMask == 0 || barrier.srcStageMask == 0) {
		throw std::runtime_error("Transition src/dst stage mask not set!");
	}
	if (barrier.dstAccessMask == 0 || barrier.srcAccessMask == 0) {
		throw std::runtime_error("Transition src/dst access mask not set!");
	}
	*/
	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType =VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.pNext = nullptr;
	dependencyInfo.dependencyFlags = 0;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(cmd, &dependencyInfo);

}

VkImageAspectFlags getAspectFlags(VkFormat format) {
	if (format == VK_FORMAT_D32_SFLOAT ||
		format == VK_FORMAT_D16_UNORM)
	{
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	if (format == VK_FORMAT_D24_UNORM_S8_UINT ||
		format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		format == VK_FORMAT_D16_UNORM_S8_UINT)
	{
		return VK_IMAGE_ASPECT_DEPTH_BIT |
			VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	return VK_IMAGE_ASPECT_COLOR_BIT;
}

void beginRendering(VulkanContext &ctx, VkCommandBuffer cmd, uint32_t imageIndex) {
	VkRenderingAttachmentInfo renderAttachInfo{};
	renderAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	renderAttachInfo.imageView = ctx.swapchainContext.imageViews[imageIndex];
	renderAttachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	renderAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	renderAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	VkClearValue clearColor{};
	clearColor.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	renderAttachInfo.clearValue = clearColor;

	VkRenderingInfo renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderInfo.renderArea = {
		{0, 0},
		ctx.swapchainContext.extent
	};
	renderInfo.layerCount = 1;
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = &renderAttachInfo;

	vkCmdBeginRendering(cmd, &renderInfo);

}
void setViewport(VulkanContext& ctx, VkCommandBuffer cmd) {
	VkViewport viewportInfo{};
	viewportInfo.width = ctx.swapchainContext.extent.width;
	viewportInfo.height = ctx.swapchainContext.extent.height;
	viewportInfo.minDepth = 0;
	viewportInfo.maxDepth = 1;
	viewportInfo.x = 0;
	viewportInfo.y = 0;
	
	vkCmdSetViewport(cmd, 0, 1, &viewportInfo);
}
void setScissor(VulkanContext& ctx, VkCommandBuffer cmd) {
	VkRect2D scissor{};
	scissor.extent = ctx.swapchainContext.extent;
	scissor.offset = { 0,0 };
	vkCmdSetScissor(cmd, 0, 1, &scissor);
}
void bindShaders(VulkanContext& ctx, VkCommandBuffer cmd) {
	static const VkShaderStageFlagBits stages[] = {
	VK_SHADER_STAGE_VERTEX_BIT,
	VK_SHADER_STAGE_FRAGMENT_BIT
};
	static const VkShaderEXT shaders[] = {
	ctx.shaderContext.vertexShader,
	ctx.shaderContext.fragmentShader

};

	PFN_vkCmdBindShadersEXT CmdBindShadersEXT = loadDeviceFunction<PFN_vkCmdBindShadersEXT>(ctx.device, "vkCmdBindShadersEXT");
	CmdBindShadersEXT(cmd, static_cast<uint32_t>(std::size(stages)), stages, shaders);

}


void endRendering(VkCommandBuffer cmd) {
	vkCmdEndRendering(cmd);
}
void endCommandBuffer(VkCommandBuffer cmd) {
	if(vkEndCommandBuffer(cmd)!= VK_SUCCESS) { throw std::runtime_error("EndCommandBuffer failed"); }
}

