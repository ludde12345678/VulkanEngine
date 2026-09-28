#include "CmdRecording.h"
#include <glm/gtc/matrix_transform.hpp>
#include "VResources.h"
#include "RenderStates.h"



void renderScene(VkCommandBuffer cmd, RendererContext& renderCtx, Scene& scene, ResourceContext& resources)
{
	VulkanContext& ctx = renderCtx.vulkanContext;
	for (auto& obj : scene.objects) {
		if (!obj.mesh.has_value()) { continue; }
		if (!obj.mesh.value().mesh.isValid) { continue; }
		Mesh& mesh = resources.meshes[obj.mesh.value().mesh.meshIndex];

		PushConstants pc{};

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, obj.transform.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, obj.transform.scale);
		pc.modelMatrix = modelMatrix;
		if (obj.mesh.value().material.has_value()) {
			// create material
			pc.materialIndex = obj.mesh.value().material.value().materialIndex;
		}
		vkCmdPushConstants(cmd, renderCtx.pipeline.pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstants),
			&pc);
		drawMesh(cmd, mesh);
	}
}


void beginRendering(VulkanContext& ctx, VkCommandBuffer cmd, std::optional <RenderTarget> color, std::optional <RenderTarget> depth) {

	VkClearValue clearColor{};
	clearColor.color = VConfig::CLEAR_COLOR_VALUE;

	VkClearValue clearDepth{};
	clearDepth.depthStencil.depth = 1.0f;
	clearDepth.depthStencil.stencil = 0;

	VkRenderingAttachmentInfo colorAttachInfo{};
	VkRenderingAttachmentInfo depthAttachInfo{};

	if (color) {
		colorAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachInfo.imageView = color->imageView,
		colorAttachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachInfo.clearValue = clearColor;
	}


	if (depth) {
		depthAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachInfo.imageView = depth->imageView,
		depthAttachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachInfo.clearValue = clearDepth;
	}


	VkRenderingInfo renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	if (color)
		renderInfo.renderArea.extent = color->extent;
	else if (depth)
		renderInfo.renderArea.extent = depth->extent;
	renderInfo.layerCount = 1;

	if (color)
	{
		renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachments = &colorAttachInfo;
	}

	if (depth)
	{
		renderInfo.pDepthAttachment = &depthAttachInfo;
	}

	vkCmdBeginRendering(cmd, &renderInfo);

}
void setViewport(VulkanContext& ctx, VkCommandBuffer cmd, RenderTarget target) {
	VkViewport viewportInfo{};
	viewportInfo.width = target.extent.width;
	viewportInfo.height = target.extent.height;
	viewportInfo.minDepth = 0;
	viewportInfo.maxDepth = 1;
	viewportInfo.x = 0;
	viewportInfo.y = 0;

	vkCmdSetViewportWithCountEXT(cmd, 1, &viewportInfo);
}
void setScissor(VulkanContext& ctx, VkCommandBuffer cmd, RenderTarget target) {
	VkRect2D scissor{};
	scissor.extent = target.extent;
	scissor.offset = { 0,0 };
	vkCmdSetScissorWithCountEXT(cmd, 1, &scissor);
}



void endRendering(VkCommandBuffer cmd) {
	vkCmdEndRendering(cmd);
}