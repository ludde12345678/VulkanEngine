#include "Desciptors.h"


void setupDescriptorResources(RendererContext& ctx)
{
	ctx.Uniforms.all =
	{
		&ctx.Uniforms.time,
		&ctx.Uniforms.camera,
		&ctx.Uniforms.shading,
	};
	ctx.Uniforms.time.info = UniformInfo
	{
		.name = "Time",
		.binding = 0,
		.set = 0,
		.size = sizeof(TimeUBO),
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Uniforms.camera.info = UniformInfo
	{
		.name = "Camera",
		.binding = 1,
		.set = 0,
		.size = sizeof(CameraUBO),
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT
	};
	ctx.Uniforms.shading.info = UniformInfo
	{
		.name = "Shading",
		.binding = 2,
		.set = 0,
		.size = sizeof(ShadingUBO),
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_FRAGMENT_BIT
	};

	// push constants
	ctx.Uniforms.pushConstants =
	{
		{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.offset = 0,
			.size = sizeof(glm::mat4)
		}
	};

	createDescriptorLayouts(ctx);
	createDescriptorPool(ctx);
	createPipelineLayout(ctx);

	createUniformBuffers(ctx);

	allocateDescriptorSets(ctx);
	updateDescriptorSets(ctx);
}

void createDescriptorLayouts(RendererContext& ctx)
{
	uint32_t setCount = 0;
	for (const auto* uniform : ctx.Uniforms.all) {
		setCount = std::max(setCount, uniform->info.set + 1);
	}
	std::vector<VkDescriptorSetLayout> setLayouts(setCount);

	for (size_t i = 0; i < setCount; i++)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		for (const auto* uniform : ctx.Uniforms.all) {
			if (uniform->info.set != i) { continue; }
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = uniform->info.binding;
			binding.descriptorType = uniform->info.type;
			binding.descriptorCount = 1;
			binding.stageFlags = uniform->info.stages;
			bindings.push_back(binding);
		}
		VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutCreateInfo.pBindings = bindings.data();

		VkDescriptorSetLayout descriptorLayout{};
		VK_CHECK(vkCreateDescriptorSetLayout(ctx.vulkanContext.device, &layoutCreateInfo, nullptr, &descriptorLayout), "Error: DescriptorLayouts failed");
		setLayouts[i] = descriptorLayout;
	}
	ctx.Uniforms.layouts = std::move(setLayouts);

}
void destroyDescriptorLayouts(RendererContext& ctx) {
	for(auto layout : ctx.Uniforms.layouts)
	vkDestroyDescriptorSetLayout(ctx.vulkanContext.device, layout, nullptr);
}
void createDescriptorPool(RendererContext& ctx) {

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(ctx.Uniforms.all.size());

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 1;
	createInfo.pPoolSizes = &poolSize;
	createInfo.maxSets = static_cast<uint32_t>(ctx.Uniforms.layouts.size());

	VkDescriptorPool descPool;
	VK_CHECK(vkCreateDescriptorPool(ctx.vulkanContext.device, &createInfo, nullptr, &descPool), "ERROR: Failed to create descriptor pool");
	ctx.DescPool = descPool;

	// create imgui desc pool
	VkDescriptorPoolSize poolSizes[] =
	{
	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
	};

	VkDescriptorPoolCreateInfo ImGuicreateInfo{};
	ImGuicreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	ImGuicreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	ImGuicreateInfo.maxSets = 1000;
	ImGuicreateInfo.poolSizeCount = 3;
	ImGuicreateInfo.pPoolSizes = poolSizes;

	VkDescriptorPool ImguidescPool;
	VK_CHECK(vkCreateDescriptorPool(ctx.vulkanContext.device, &ImGuicreateInfo, nullptr, &ImguidescPool), "ERROR: Failed to create ImGui descriptor pool");
	ctx.ImGuiDescPool = ImguidescPool;
}

void allocateDescriptorSets(RendererContext& ctx)
{


	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = ctx.DescPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(ctx.Uniforms.layouts.size());
	allocInfo.pSetLayouts = ctx.Uniforms.layouts.data();

	std::vector<VkDescriptorSet> sets(static_cast<uint32_t>(ctx.Uniforms.layouts.size()));
	VK_CHECK(vkAllocateDescriptorSets(ctx.vulkanContext.device, &allocInfo, sets.data()), "Error: Failed to allocate descriptor set");

	ctx.Uniforms.sets = std::move(sets);
}

void updateDescriptorSets(RendererContext& ctx) {

	std::vector<VkWriteDescriptorSet> writes;
	std::vector<VkDescriptorBufferInfo> bufferInfos;

	bufferInfos.reserve(ctx.Uniforms.all.size());
	writes.reserve(ctx.Uniforms.all.size());

	for (auto& uniform : ctx.Uniforms.all) {
		VkDescriptorBufferInfo bufferInfo{};

		bufferInfo.buffer = uniform->buffer.buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = uniform->info.size;

		bufferInfos.push_back(bufferInfo);
	
		VkWriteDescriptorSet write{};

		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = ctx.Uniforms.sets[uniform->info.set];
		write.dstBinding = uniform->info.binding;
		write.descriptorType = uniform->info.type;
		write.descriptorCount = 1;
		write.pBufferInfo = &bufferInfos.back();
		writes.push_back(write);
	
	}

	vkUpdateDescriptorSets(ctx.vulkanContext.device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

}

void destroyDescriptorPool(RendererContext& ctx)
{
	vkDestroyDescriptorPool(
		ctx.vulkanContext.device,
		ctx.DescPool,
		nullptr
	);
	vkDestroyDescriptorPool(
		ctx.vulkanContext.device,
		ctx.ImGuiDescPool,
		nullptr
	);
}


void createUniformBuffers(RendererContext& ctx)
{
	for (auto* uniform : ctx.Uniforms.all) {
		uniform->buffer = createUniformBuffer(ctx.vulkanContext, uniform->info.size);
	
	}
}
void destroyUniformBuffers(RendererContext& ctx)
{
	for (auto* uniform : ctx.Uniforms.all) {
		destroyBuffer(ctx.vulkanContext, uniform->buffer);

	}
}

void bindUniformBuffers(RendererContext& ctx, VkCommandBuffer cmd) {

		vkCmdBindDescriptorSets(
			cmd,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			ctx.pipeline.pipelineLayout,
			0,
			ctx.Uniforms.sets.size(),
			ctx.Uniforms.sets.data(),
			0,
			nullptr
		);

}