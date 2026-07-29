#include "Uniforms.h"




UniformBuffers AllocateUniformBuffers(VulkanContext& ctx)
{

	UniformBuffers buffers{};
	buffers.timeUB = createUniformBuffer(ctx, sizeof(TimeUBO));
	return buffers;

}

void bindUniformBuffers(RendererContext& ctx, VkCommandBuffer cmd) {


	vkCmdBindDescriptorSets(
		cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		ctx.pipeline.pipelineLayout,
		0,
		1,
		&ctx.descriptors.timeSet,
		0,
		nullptr
	);

}