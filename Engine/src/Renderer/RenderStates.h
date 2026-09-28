#pragma once
#include "./RendererContext.h"

namespace RenderStates {

	inline VulkanGraphicsState Opaque
	{
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestart = VK_FALSE,
		.lineWidth = 1.0f,

		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.rasterizerDiscard = VK_FALSE,

		.depth = VulkanDepthState{
			.testEnable = VK_TRUE,
			.writeEnable = VK_TRUE,

			.compareOp = VK_COMPARE_OP_LESS,

			.biasEnable = VK_FALSE,
			.biasInfo = VkDepthBiasInfoEXT{},

		},

		.stencil = VulkanStencilState{
			.testEnable = false,
			.front = VkStencilOpState{},
			.back = VkStencilOpState{}
		},

		.samples = VK_SAMPLE_COUNT_1_BIT,
		.sampleMask = 0xFFFFFFFF,
		.alphaToCoverage = VK_FALSE,


		.colorBlendEnables = std::vector<VkBool32>
		{VK_FALSE},
		.blendEquations = std::vector<VkColorBlendEquationEXT>
		{
			VkColorBlendEquationEXT{
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor =
				VK_BLEND_FACTOR_ZERO,
			.colorBlendOp =
				VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor =
				VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor =
				VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp =
				VK_BLEND_OP_ADD,
			}
		},
		.colorWriteMasks = std::vector<VkColorComponentFlags>
		{
			{
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT
			}
		},

	};	
	inline VulkanGraphicsState Shadow
	{
		// Assembly
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestart = VK_FALSE,
		.lineWidth = 1.0f,

		// Rasterisation
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.rasterizerDiscard = VK_FALSE,

		// Depth
		.depth = VulkanDepthState{
			.testEnable = VK_TRUE,
			.writeEnable = VK_TRUE,
			.compareOp = VK_COMPARE_OP_LESS,

			.biasEnable = VK_TRUE,
			.biasInfo = VkDepthBiasInfoEXT{
				.sType = VK_STRUCTURE_TYPE_DEPTH_BIAS_INFO_EXT,
				.depthBiasConstantFactor = 1.0f,
				.depthBiasClamp = 0.0f,
				.depthBiasSlopeFactor = 1.0f
			}
		},

		// Stencil
		.stencil = VulkanStencilState{
			.testEnable = VK_FALSE,
			.front = {},
			.back = {}
		},

		// MSAA
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.sampleMask = 0xFFFFFFFF,
		.alphaToCoverage = VK_FALSE,

		// Color
		.colorBlendEnables = {},
		.blendEquations = {},
		.colorWriteMasks = {}
	};



}