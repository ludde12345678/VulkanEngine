#include "FrameRenderer.h"




void DrawFrame(RendererContext& RenderCtx, ResourceContext& resources, GLFWwindow* window, Scene& scene, RenderData& Rdata) {

	VulkanContext& ctx = RenderCtx.vulkanContext;
	waitForFrame(ctx);
	uint32_t imageIndex = getSwapchainImage(ctx);
	if (ctx.swapchainContext.status == SwapchainStatus::Invalid)
	{
		recreateSwapchainResources(ctx, window);
		return;
	}
	vkResetFences(ctx.device, 1, &ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame]);

	// update all uniforms;
	updatePerFrameUniformBuffers(RenderCtx, Rdata);


	recordCmdBuffer(RenderCtx, resources, imageIndex, &scene);




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

	if (res == VK_SUCCESS) { return imageIndex; }
	else if (res == VK_SUBOPTIMAL_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Suboptimal Swapchain\n"; }
		ctx.swapchainContext.status = SwapchainStatus::NeedsRecreation;
		return imageIndex;
	}
	else if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Invalid Swapchain\n"; }
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
	signalSemInfo.semaphore = ctx.syncContext.renderFinishedSemaphores[imageIndex];
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
	VK_CHECK(
		vkQueueSubmit2(
			ctx.queueContext.graphicsQueue,
			1,
			&submitInfo,
			ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame]
		),
		"Queue submit failed"
	);

}
void presentFrame(VulkanContext& ctx, uint32_t imageIndex) {
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	// wait for the renderFinished semaphore to signal, aka wait for image to finish rendering
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &ctx.syncContext.renderFinishedSemaphores[imageIndex];

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &ctx.swapchainContext.swapchain;

	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkResult res = vkQueuePresentKHR(ctx.queueContext.presentQueue, &presentInfo);

	if (res == VK_SUCCESS) { return; }
	else if (res == VK_SUBOPTIMAL_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Suboptimal Swapchain\n"; }
		ctx.swapchainContext.status = SwapchainStatus::NeedsRecreation;
		return;
	}
	else if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Invalid Swapchain\n"; }
		ctx.swapchainContext.status = SwapchainStatus::Invalid;
		return;
	}
	else {
		throw std::runtime_error("Error: Unknown error in acquire swapchain image");
	}

}

void recreateSwapchainResources(VulkanContext& ctx, GLFWwindow* window)
{
	int width = 0;
	int height = 0;

	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(ctx.device);
	recreateSwapchain(ctx, window);
	recreateImageSync(ctx);
	VkCommandBuffer tempcmd = beginSingleTimeCommands(ctx);
	transitionSwapchainDepthImage(ctx, tempcmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	endSingleTimeCommands(ctx, tempcmd);
}