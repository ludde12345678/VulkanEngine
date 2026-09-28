#include "RenderStages.h"


void GeometryPass(VkCommandBuffer cmd, RendererContext& renderCtx, ResourceContext& resources, Scene& scene, RenderTarget& target, RenderTarget& depthTarget, const Settings& settings) {
	VulkanContext& ctx = renderCtx.vulkanContext;
	
	transitionImage(ctx, cmd, target, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	beginRendering(ctx, cmd, target, depthTarget);

	setViewport(ctx, cmd, target);
	setScissor(ctx, cmd, target);
	bindShaders(renderCtx, cmd, "main");

	VulkanGraphicsState state = RenderStates::Opaque;
	if (settings.debugRenderSettings.wireframe) {
		state.polygonMode = VK_POLYGON_MODE_LINE;
	}

	setupDynamicState(ctx, cmd, state);

	bindDescriptorSets(renderCtx, cmd, DescSetsMask::GLOBAL | DescSetsMask::MATERIAL | DescSetsMask::SHADOW);
	renderScene(cmd, renderCtx, scene, resources);

	if (renderCtx.ImGUIEnabled) {
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(
			ImGui::GetDrawData(),
			cmd
		);
	}

	endRendering(cmd);


}

void DebugPass(VkCommandBuffer cmd, RendererContext& renderCtx, ResourceContext& resources, Scene& scene, RenderTarget& target, const Settings& settings) {
	VulkanContext& ctx = renderCtx.vulkanContext;

	transitionImage(ctx, cmd, target, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	beginRendering(ctx, cmd, target, std::nullopt);

	setViewport(ctx, cmd, target);
	setScissor(ctx, cmd, target);
	bindShaders(renderCtx, cmd, "debug");

	VulkanGraphicsState state = RenderStates::Opaque;

	state.depth.testEnable = VK_FALSE;
	state.depth.writeEnable = VK_FALSE;
	state.cullMode = VK_CULL_MODE_NONE;
	if (settings.debugRenderSettings.wireframe) {
		state.polygonMode = VK_POLYGON_MODE_LINE;
	}

	setupDynamicState(ctx, cmd, state);
	bindDescriptorSets(renderCtx, cmd, DescSetsMask::SHADOW | DescSetsMask::GLOBAL);
	
	vkCmdDraw(cmd, 6, 1, 0, 0);

	if (renderCtx.ImGUIEnabled) {
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(
			ImGui::GetDrawData(),
			cmd
		);
	}

	endRendering(cmd);


}

void ShadowMapPass(VkCommandBuffer cmd, RendererContext& renderCtx, ResourceContext& resources, Scene& scene, RenderTarget& depthTarget, const Settings& settings) {
	VulkanContext& ctx = renderCtx.vulkanContext;

	transitionImage(ctx, cmd, depthTarget, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
	beginRendering(ctx, cmd, std::nullopt, depthTarget);

	setViewport(ctx, cmd, depthTarget);
	setScissor(ctx, cmd, depthTarget);
	bindShaders(renderCtx, cmd, "shadow");

	VulkanGraphicsState state = RenderStates::Shadow;


	setupDynamicState(ctx, cmd, state);

	bindDescriptorSets(renderCtx, cmd, DescSetsMask::GLOBAL | DescSetsMask::SHADOW);
	renderScene(cmd, renderCtx, scene, resources);


	endRendering(cmd);


}
