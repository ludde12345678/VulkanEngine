#include "Desciptors.h"




void createDescriptorLayouts(RendererContext& ctx)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = 0;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = 1;
	layoutCreateInfo.pBindings = &binding;

	VkDescriptorSetLayout descriptorLayout{};
	vkCreateDescriptorSetLayout(ctx.vulkanContext.device, &layoutCreateInfo, nullptr, &descriptorLayout);

	ctx.descriptors.timeLayout = descriptorLayout;


}
void destroyDescriptorLayouts(RendererContext& ctx) {
	vkDestroyDescriptorSetLayout(ctx.vulkanContext.device, ctx.descriptors.timeLayout, nullptr);
}
void createDescriptorPool(RendererContext& ctx) {

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 1;
	createInfo.pPoolSizes = &poolSize;
	createInfo.maxSets = 1;

	VkDescriptorPool descPool;
	VK_CHECK(vkCreateDescriptorPool(ctx.vulkanContext.device, &createInfo, nullptr, &descPool), "ERROR: Failed to create descriptor pool");
	ctx.descriptors.pool = descPool;
}

void allocateDescriptorSets(RendererContext& ctx)
{


	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = ctx.descriptors.pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &ctx.descriptors.timeLayout;

	VkDescriptorSet timeSet;
	VK_CHECK(vkAllocateDescriptorSets(ctx.vulkanContext.device, &allocInfo, &timeSet), "Error: Failed to allocate descriptor set");

	ctx.descriptors.timeSet = timeSet;
}

void updateDescriptorSets(RendererContext& ctx) {

	VkDescriptorBufferInfo bufferInfo{};

	bufferInfo.buffer = ctx.Uniforms.timeUB.buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(TimeUBO);

	VkWriteDescriptorSet write{};

	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = ctx.descriptors.timeSet;
	write.dstBinding = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.descriptorCount = 1;
	write.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(ctx.vulkanContext.device, 1, &write, 0, nullptr);

}

void destroyDescriptorPool(RendererContext& ctx)
{
	vkDestroyDescriptorPool(
		ctx.vulkanContext.device,
		ctx.descriptors.pool,
		nullptr
	);
}