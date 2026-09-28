#include "FrameRenderer.h"
#include "RendererInternal.h"
#include "RenderStageResources.h"
// creates the render targets, underlying images, and sets the lights shaowmapIndex, needs the lights array filled!
void initShadowMapRenderTargets(RendererContext& RenderCtx, RenderData& data, ResourceContext& resources) {

	// Shadow maps
	resources.ShadowMapTextures.reserve(VConfig::MAX_SHADOW_MAPS);
	uint32_t createdShadowmaps = 0;
	RenderStageImageConfig shadowMapConf = RenderStageResources::shadowMapConfig;
	for (Light& L : data.StorageBuffers.lights) {
		if (L.castsShadow) {
			// image
			VkImage ShadowMapImage;
			VK_CHECK(vkCreateImage(RenderCtx.vulkanContext.device, &shadowMapConf.image, nullptr, &ShadowMapImage), "Error creating ShadowMap");
			if (ShadowMapImage == VK_NULL_HANDLE) { throw std::runtime_error("ShadowMap image not created"); }
			// allocate
			AllocatedImage allocShadowMapImage = allocateImage(RenderCtx.vulkanContext, ShadowMapImage);
			allocShadowMapImage.format = shadowMapConf.image.format;
			allocShadowMapImage.extent = { shadowMapConf.image.extent.width, shadowMapConf.image.extent.height };
			// imageView
			shadowMapConf.view.image = ShadowMapImage;
			VkImageView ShadowView;
			if (vkCreateImageView(RenderCtx.vulkanContext.device, &shadowMapConf.view, nullptr, &ShadowView) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create Shadowmap image view");
			}
			allocShadowMapImage.view = ShadowView;
			// sampler
			Texture tex{
			.image = allocShadowMapImage
			};

			if (shadowMapConf.sampler.has_value()) {
				VK_CHECK(vkCreateSampler(RenderCtx.vulkanContext.device, &shadowMapConf.sampler.value(), nullptr, &tex.sampler), "Failed to create shadowmap sampler");
			}

			RenderTarget shadowMapTarget{
				.image = allocShadowMapImage.image,
				.imageView = allocShadowMapImage.view,
				.format = allocShadowMapImage.format,
				.extent = allocShadowMapImage.extent,
				.layout = VK_IMAGE_LAYOUT_UNDEFINED
			};
			RenderCtx.targets.shadowMaps.push_back(shadowMapTarget);
			RenderCtx.targets.images.push_back(allocShadowMapImage);
			resources.ShadowMapTextures.push_back(tex);
			size_t index = resources.ShadowMapTextures.size() - 1;
			L.shadowMapIndex = index;
			// create texture 
			// this is the magic line, remember
			RenderCtx.Descriptors.ShadowMaps.textures.push_back(&resources.ShadowMapTextures[index]);
			createdShadowmaps++;
		}
	}
	// Create default/unused shadowmap textures to make GLSL happy
	VkImage DefaultShadowMapImage;
	VK_CHECK(vkCreateImage(RenderCtx.vulkanContext.device, &shadowMapConf.image, nullptr, &DefaultShadowMapImage), "Error creating Default ShadowMap");
	if (DefaultShadowMapImage == VK_NULL_HANDLE) { throw std::runtime_error("Default ShadowMap image not created"); }
	// allocate
	AllocatedImage DefaultallocShadowMapImage = allocateImage(RenderCtx.vulkanContext, DefaultShadowMapImage);
	DefaultallocShadowMapImage.format = shadowMapConf.image.format;
	DefaultallocShadowMapImage.extent = { shadowMapConf.image.extent.width, shadowMapConf.image.extent.height };
	// imageView
	shadowMapConf.view.image = DefaultShadowMapImage;
	VkImageView DefaultShadowView;
	if (vkCreateImageView(RenderCtx.vulkanContext.device, &shadowMapConf.view, nullptr, &DefaultShadowView) != VK_SUCCESS)
	{throw std::runtime_error("Failed to create Default Shadowmap image view");}
	DefaultallocShadowMapImage.view = DefaultShadowView;
	VkSampler Defaultsampler;
	if (shadowMapConf.sampler.has_value()) {
		VK_CHECK(vkCreateSampler(RenderCtx.vulkanContext.device, &shadowMapConf.sampler.value(), nullptr, &Defaultsampler), "Failed to create Default shadowmap sampler");
	}
	RenderCtx.targets.images.push_back(DefaultallocShadowMapImage);
	VkCommandBuffer tempcmd = beginSingleTimeCommands(RenderCtx.vulkanContext);
	transitionImageLayout(
		tempcmd,
		DefaultShadowMapImage,
		shadowMapConf.image.format,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL
	);
	// TODO: fix this so that the default shadowmap is init to 1
	endSingleTimeCommands(RenderCtx.vulkanContext, tempcmd);

	// Create default shadowmap texture
	Texture defTex{
	.image = DefaultallocShadowMapImage,
	.sampler = Defaultsampler
	};
	for (uint32_t i = 0; i < VConfig::MAX_SHADOW_MAPS - createdShadowmaps; i++) {

		// fill it in
		resources.ShadowMapTextures.push_back(defTex);
		size_t index = resources.ShadowMapTextures.size() - 1;
		RenderCtx.Descriptors.ShadowMaps.textures.push_back(&resources.ShadowMapTextures[index]);
	}

}

void DrawFrame(RendererContext& RenderCtx, ResourceContext& resources, GLFWwindow* window, Scene& scene, RenderData& Rdata, const Settings& settings) {

	VulkanContext& ctx = RenderCtx.vulkanContext;
	waitForFrame(ctx);
	uint32_t imageIndex = getSwapchainImage(ctx);
	if (ctx.swapchainContext.status == SwapchainStatus::Invalid){recreateSwapchainResources(ctx, window);return;}

	vkResetFences(ctx.device, 1, &ctx.syncContext.inFlightFences[ctx.syncContext.currentFrame]);
	// update all uniforms;
	updatePerFrameUniformBuffers(RenderCtx, Rdata);

	VkCommandBuffer cmd = ctx.cmdBuffers[imageIndex];
	beginCommandBuffer(cmd);
	// Render passes
	/*
		uint32_t lightIndex = 0;
	vkCmdPushConstants(
		cmd,
		RenderCtx.pipeline.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		sizeof(PushConstants),
		sizeof(uint32_t),
		&lightIndex
	);
	ShadowMapPass(cmd, RenderCtx, resources, scene, RenderCtx.targets.shadowMaps[0], settings);
	transitionImage(ctx, cmd, RenderCtx.targets.shadowMaps[0], VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
	*/
	uint32_t lightIndex = 0;
	if (settings.renderSettings.shadowMap) {
		for (RenderTarget& target : RenderCtx.targets.shadowMaps) {
			lightIndex += 1;
			vkCmdPushConstants(
				cmd,
				RenderCtx.pipeline.pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				sizeof(PushConstants),
				sizeof(uint32_t),
				&lightIndex
			);

			ShadowMapPass(cmd, RenderCtx, resources, scene, target, settings);
			transitionImage(ctx, cmd, target, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
		}
	}

	RenderTarget geometryTarget = ctx.swapchainContext.colorTargets[imageIndex];
	RenderTarget geometryDepthTarget = ctx.swapchainContext.depthTargets[imageIndex];
	//DebugPass(cmd, RenderCtx, resources, scene, geometryTarget, settings);
	GeometryPass(cmd, RenderCtx, resources, scene, geometryTarget, geometryDepthTarget, settings);

	

	transitionImage(ctx, cmd, geometryTarget, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	endCommandBuffer(cmd);
	// present frame
	
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


	// submit work, when finished, signal inflight fence, so cpu know this frameslot is cleared for reuse
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