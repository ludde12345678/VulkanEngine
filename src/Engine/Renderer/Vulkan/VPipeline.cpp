#include "VPipeline.h"


void setupDynamicState(VulkanContext& ctx, VkCommandBuffer cmd)
{
	vkCmdSetPrimitiveTopologyEXT(
		cmd,
		ctx.currentGraphicsState.topology
	);

	vkCmdSetCullModeEXT(
		cmd,
		ctx.currentGraphicsState.cullMode
	);
	vkCmdSetFrontFaceEXT(
		cmd,
		ctx.currentGraphicsState.frontFace
	);
	vkCmdSetPolygonModeEXT(
		cmd,
		ctx.currentGraphicsState.polygonMode
	);
	vkCmdSetRasterizerDiscardEnableEXT(
		cmd,
		ctx.currentGraphicsState.rasterizerDiscard
	);
	vkCmdSetDepthTestEnableEXT(
		cmd,
		ctx.currentGraphicsState.depthTest
	);
	vkCmdSetDepthBiasEnableEXT(
		cmd,
		ctx.currentGraphicsState.depthBias
	);
	vkCmdSetDepthWriteEnableEXT(
		cmd,
		ctx.currentGraphicsState.depthWrite
	);
	vkCmdSetStencilTestEnableEXT(
		cmd,
		ctx.currentGraphicsState.stencilTest
	);
	vkCmdSetPrimitiveRestartEnableEXT(
		cmd,
		VK_FALSE
	);
	vkCmdSetRasterizationSamplesEXT(
		cmd,
		ctx.currentGraphicsState.samples
	);
	vkCmdSetSampleMaskEXT(
		cmd,
		ctx.currentGraphicsState.samples,
		&ctx.currentGraphicsState.sampleMask
	);
	vkCmdSetAlphaToCoverageEnableEXT(
		cmd,
		ctx.currentGraphicsState.alphaToCoverage
	);
	vkCmdSetColorBlendEnableEXT(
		cmd,
		0,          // first attachment
		1,          // number of attachments
		&ctx.currentGraphicsState.colorBlendEnable
	);

	VkColorBlendEquationEXT blendEquation{};

	blendEquation.srcColorBlendFactor =
		VK_BLEND_FACTOR_ONE;
	blendEquation.dstColorBlendFactor =
		VK_BLEND_FACTOR_ZERO;
	blendEquation.colorBlendOp =
		VK_BLEND_OP_ADD;

	blendEquation.srcAlphaBlendFactor =
		VK_BLEND_FACTOR_ONE;
	blendEquation.dstAlphaBlendFactor =
		VK_BLEND_FACTOR_ZERO;
	blendEquation.alphaBlendOp =
		VK_BLEND_OP_ADD;

	vkCmdSetColorBlendEquationEXT(
		cmd,
		0,
		1,
		&blendEquation
	);

	VkColorComponentFlags mask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	vkCmdSetColorWriteMaskEXT(
		cmd,
		0,
		1,
		&mask
	);

}