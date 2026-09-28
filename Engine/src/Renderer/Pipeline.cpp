#include "Pipeline.h"


void setupDynamicState(VulkanContext& ctx, VkCommandBuffer cmd, VulkanGraphicsState state)
{
	// assembly
	vkCmdSetPrimitiveTopologyEXT(cmd,state.topology);
	vkCmdSetLineWidth(cmd, state.lineWidth);
	vkCmdSetPrimitiveRestartEnableEXT(cmd,state.primitiveRestart);

	// rasterization
	vkCmdSetCullModeEXT(cmd,state.cullMode);
	vkCmdSetFrontFaceEXT(cmd,state.frontFace);
	vkCmdSetPolygonModeEXT(cmd,state.polygonMode);
	vkCmdSetRasterizerDiscardEnableEXT(cmd,state.rasterizerDiscard);

	// Depth
	vkCmdSetDepthTestEnableEXT(cmd,state.depth.testEnable);
	vkCmdSetDepthWriteEnableEXT(cmd, state.depth.writeEnable);
	vkCmdSetDepthCompareOpEXT(cmd,state.depth.compareOp);


	// depth bias
	vkCmdSetDepthBiasEnableEXT(cmd,state.depth.biasEnable);

	vkCmdSetDepthBias(
		cmd,
		state.depth.biasInfo.depthBiasConstantFactor,
		state.depth.biasInfo.depthBiasClamp,
		state.depth.biasInfo.depthBiasSlopeFactor
	);

	// Stencil
	vkCmdSetStencilTestEnable(cmd,state.stencil.testEnable);
	// front face
	vkCmdSetStencilOpEXT(cmd, VK_STENCIL_FACE_FRONT_BIT, state.stencil.front.failOp, state.stencil.front.passOp, state.stencil.front.depthFailOp, state.stencil.front.compareOp);
	vkCmdSetStencilCompareMask(cmd, VK_STENCIL_FACE_FRONT_BIT, state.stencil.front.compareMask);
	vkCmdSetStencilWriteMask(cmd, VK_STENCIL_FACE_FRONT_BIT, state.stencil.front.writeMask);
	vkCmdSetStencilReference(cmd, VK_STENCIL_FACE_FRONT_BIT, state.stencil.front.reference);
	//back face
	vkCmdSetStencilOpEXT(cmd, VK_STENCIL_FACE_BACK_BIT, state.stencil.back.failOp, state.stencil.back.passOp, state.stencil.back.depthFailOp, state.stencil.back.compareOp);
	vkCmdSetStencilCompareMask(cmd, VK_STENCIL_FACE_BACK_BIT, state.stencil.back.compareMask);
	vkCmdSetStencilWriteMask(cmd, VK_STENCIL_FACE_BACK_BIT, state.stencil.back.writeMask);
	vkCmdSetStencilReference(cmd, VK_STENCIL_FACE_BACK_BIT, state.stencil.back.reference);


	// MSAA
	vkCmdSetRasterizationSamplesEXT(cmd,state.samples);
	vkCmdSetSampleMaskEXT(cmd,state.samples,&state.sampleMask);
	vkCmdSetAlphaToCoverageEnableEXT(cmd,state.alphaToCoverage);

	// Color blend
	if (!state.colorBlendEnables.empty()) {
		vkCmdSetColorBlendEnableEXT(cmd, 0, state.colorBlendEnables.size(), state.colorBlendEnables.data());
		vkCmdSetColorBlendEquationEXT(cmd, 0, state.blendEquations.size(), state.blendEquations.data());
		vkCmdSetColorWriteMaskEXT(cmd, 0, state.colorWriteMasks.size(), state.colorWriteMasks.data());
	}
}

void createPipelineLayout(RendererContext& ctx) {


	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	// descriptors
	layoutCreateInfo.setLayoutCount = ctx.Descriptors.layouts.size();
	layoutCreateInfo.pSetLayouts = ctx.Descriptors.layouts.data();

	//push constants
	layoutCreateInfo.pushConstantRangeCount = ctx.Descriptors.pushConstants.size();
	layoutCreateInfo.pPushConstantRanges = ctx.Descriptors.pushConstants.data();


	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(ctx.vulkanContext.device, &layoutCreateInfo, nullptr, &layout) != VK_SUCCESS) { throw std::runtime_error("Error: Pipeline layout creation failed"); }

	PipelineContext pipeCtx{};
	pipeCtx.pipelineLayout = layout;

	ctx.pipeline = pipeCtx;

}
void destroyPipelineLayout(RendererContext& ctx) {

	vkDestroyPipelineLayout(ctx.vulkanContext.device, ctx.pipeline.pipelineLayout, nullptr);

}
