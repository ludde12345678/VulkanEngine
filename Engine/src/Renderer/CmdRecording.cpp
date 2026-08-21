#include "CmdRecording.h"
#include <glm/gtc/matrix_transform.hpp>
#include "VResources.h"


void recordCmdBuffer(RendererContext& renderCtx, ResourceContext& resources, uint32_t imageIndex, Scene* scene) {

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

	bindDescriptorSets(renderCtx, cmd);



	for (auto& obj : scene->objects) {
		if (!obj.mesh.isValid) { continue; }
		Mesh& mesh = resources.meshes[obj.mesh.meshIndex];

		PushConstants pc{};

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, obj.position);
		modelMatrix = glm::rotate(modelMatrix, obj.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, obj.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, obj.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, obj.scale);
		pc.modelMatrix = modelMatrix;
		
		pc.AlbedoIndex = obj.material.AlbedoIndex.textureIndex;
		pc.NormalIndex = obj.material.NormalIndex.textureIndex;
		pc.color = obj.material.color;
		pc.RoughnessIndex = obj.material.RoughnessIndex.textureIndex;
		pc.specularIntensity = obj.material.specularIntensity;

		vkCmdPushConstants(cmd, renderCtx.pipeline.pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstants),
			&pc);
		drawMesh(cmd, mesh);
	}

	if (renderCtx.ImGUIEnabled) {
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


void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex) {

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