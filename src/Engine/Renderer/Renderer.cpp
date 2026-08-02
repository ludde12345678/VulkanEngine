#include "Renderer.h"
#include "../App/appConfig.h"

void InitializeVulkan(RendererContext &RenderCtx ,GLFWwindow* window) {
	VulkanContext& ctx = RenderCtx.vulkanContext;
	volkInitialize();
	ctx.currentGraphicsState = VConfig::DefaultGraphicsState;

	createInstance(ctx);
	volkLoadInstance(ctx.instance);
	createDebugMessenger(ctx);
	createSurface(ctx, window);
	pickPhysicalDevice(ctx);
	createLogicalDevice(ctx);
	volkLoadDevice(ctx.device);
	createSwapChain(ctx, window);
	createCommandPool(ctx);
	createCommandBuffers(ctx);
	VkCommandBuffer tempcmd = beginSingleTimeCommands(ctx);
	transitionSwapchainDepthImage(ctx, tempcmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	endSingleTimeCommands(ctx, tempcmd);
	initializeSync(ctx);

	setupDescriptorResources(RenderCtx);
	createShaders(RenderCtx);

}

void DestroyVulkan(RendererContext& ctx, Scene scene) {

	vkDeviceWaitIdle(ctx.vulkanContext.device);
	destroyUniformBuffers(ctx);
	for (auto& mesh : scene.meshes) {
		destroyBuffer(ctx.vulkanContext, mesh.vertexBuffer);
		destroyBuffer(ctx.vulkanContext, mesh.indexBuffer);
	}

	ImGui_ImplVulkan_Shutdown();
	destroyShaders(ctx);
	destroyDescriptorPool(ctx);
	destroyPipelineLayout(ctx);
	destroyDescriptorLayouts(ctx);
	


	destroySync(ctx.vulkanContext);
	destroyCommandPool(ctx.vulkanContext);
	destroySwapchain(ctx.vulkanContext);
	vkDestroyDevice(ctx.vulkanContext.device, nullptr);
	vkDestroySurfaceKHR(ctx.vulkanContext.instance, ctx.vulkanContext.surface, nullptr);

	if (ctx.vulkanContext.debugMessenger != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(
			ctx.vulkanContext.instance,
			ctx.vulkanContext.debugMessenger,
			nullptr
		);
	}

	vkDestroyInstance(ctx.vulkanContext.instance, nullptr);
	std::cout << "\nVulkan successfully destroyed\n";
}


void printRendererDebugInfo(RendererContext& ctx, RenderData& renderData, Scene& scene) {
	std::cout << "------------------\nRenderData:";
	std::cout << "Time counter: " << renderData.Uniforms.time.FrameCount;
	std::cout << "ammount of objects: " << scene.objects.size() << "\n";
}

void DrawFrame(RendererContext& RenderCtx, GLFWwindow* window, Scene& scene, RenderData& Rdata){
	
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
	updateAllUniformBuffers(RenderCtx, Rdata);

	
	recordCmdBuffers(RenderCtx, imageIndex, &scene);
	
	
	
	
	submitFrame(ctx, imageIndex);
	presentFrame(ctx, imageIndex);
	ctx.syncContext.currentFrame = (ctx.syncContext.currentFrame + 1) % VConfig::MAX_FRAMES_IN_FLIGHT;

}
void recordCmdBuffers(RendererContext& renderCtx, uint32_t imageIndex, Scene* scene) {

	VulkanContext& ctx = renderCtx.vulkanContext;
	VkCommandBuffer cmd = ctx.cmdBuffers[imageIndex];
	//vkResetCommandBuffer(cmd, 0);
	beginCommandBuffer(cmd);
	transitionSwapchainImage(ctx, cmd, imageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	beginRendering(ctx, cmd, imageIndex);
	setViewport(ctx, cmd);
	setScissor(ctx, cmd);
	bindShaders(renderCtx, cmd);
	setupDynamicState(ctx, cmd);

	bindUniformBuffers(renderCtx, cmd);



	for (auto& obj : scene->objects) {

		Mesh& mesh = scene->meshes[obj.meshIndex];

		vkCmdPushConstants(cmd, renderCtx.pipeline.pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(obj.transform),
			&obj.transform);
		drawMesh(cmd, mesh);
	}

	if (appConfig::enableImGui) {
		// Draw UI last
		ImGui_ImplVulkan_RenderDrawData(
			ImGui::GetDrawData(),
			cmd
		);
	}
	endRendering(cmd);
	transitionSwapchainImage(ctx, cmd, imageIndex, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	endCommandBuffer(cmd);

}

void updateAllUniformBuffers(RendererContext& RenderCtx, RenderData& Rdata)
{
	updateUniformBuffer<TimeUBO>(RenderCtx.vulkanContext, RenderCtx.Uniforms.time.buffer, Rdata.Uniforms.time);
	updateUniformBuffer<CameraUBO>(RenderCtx.vulkanContext, RenderCtx.Uniforms.camera.buffer, Rdata.Uniforms.camera);
	updateUniformBuffer<ShadingUBO>(RenderCtx.vulkanContext, RenderCtx.Uniforms.shading.buffer, Rdata.Uniforms.shading);
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

void waitForFrame(VulkanContext& ctx) {

	vkWaitForFences(ctx.device, 1, &ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame], VK_TRUE, UINT64_MAX);
}
uint32_t getSwapchainImage(VulkanContext& ctx) {

	VkSemaphore imageAvailable = ctx.syncContext.imageAvailableSemaphores[ctx.syncContext.currentFrame];
	uint32_t imageIndex = 0;

	VkResult res = vkAcquireNextImageKHR(ctx.device, ctx.swapchainContext.swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

	if(res == VK_SUCCESS) { return imageIndex; }
	else if (res == VK_SUBOPTIMAL_KHR) {
		if (VConfig::PRINT_DEBUG_INFO) { std::cout << "WARNING: Suboptimal Swapchain\n"; }
		ctx.swapchainContext.status = SwapchainStatus::NeedsRecreation;
		return imageIndex;
	}
	else if (res == VK_ERROR_OUT_OF_DATE_KHR) {
		if(VConfig::PRINT_DEBUG_INFO){ std::cout << "WARNING: Invalid Swapchain\n"; }
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









void beginRendering(VulkanContext &ctx, VkCommandBuffer cmd, uint32_t imageIndex) {
	
	VkClearValue clearColor{};
	clearColor.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	
	VkRenderingAttachmentInfo colorAttachInfo{};
	colorAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachInfo.imageView = ctx.swapchainContext.imageViews[imageIndex];
	colorAttachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachInfo.clearValue = clearColor;

	VkClearValue clearDepth{};
	clearDepth.depthStencil.depth = 1.0f;
	clearDepth.depthStencil.stencil = 0;

	VkRenderingAttachmentInfo depthAttachInfo{};
	depthAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachInfo.imageView = ctx.swapchainContext.allocatedDepthImages[imageIndex].view;
	depthAttachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachInfo.clearValue = clearDepth;



	VkRenderingInfo renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderInfo.renderArea = {
		{0, 0},
		ctx.swapchainContext.extent
	};
	renderInfo.layerCount = 1;

	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = &colorAttachInfo;
	renderInfo.pDepthAttachment = &depthAttachInfo;

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
	
	vkCmdSetViewportWithCountEXT(cmd, 1, &viewportInfo);
}
void setScissor(VulkanContext& ctx, VkCommandBuffer cmd) {
	VkRect2D scissor{};
	scissor.extent = ctx.swapchainContext.extent;
	scissor.offset = { 0,0 };
	vkCmdSetScissorWithCountEXT(cmd, 1, &scissor);
}



void endRendering(VkCommandBuffer cmd) {
	vkCmdEndRendering(cmd);
}

