#include "Desciptors.h"
#include "RendererConfig.h"
#include "../EngineData.h"

void setupDescriptorResources(RendererContext& ctx, ResourceContext& resources, Scene& scene)
{
	createDescriptorInfos(ctx);

	createDescriptorLayouts(ctx);
	createDescriptorPool(ctx);
	createPipelineLayout(ctx);

	createUniformBuffers(ctx);

	allocateDescriptorSets(ctx);

	ctx.Descriptors.textures.textures.clear();
	ctx.Descriptors.textures.textures.reserve(VConfig::MAX_TEXTURES);

	uint32_t TextureCount = VConfig::MAX_TEXTURES;
	for (size_t i = 0; i < TextureCount; i++)
	{
		if (i < resources.textures.size()) {
			ctx.Descriptors.textures.textures.push_back(&resources.textures[i]);
		}
		else
		{
			ctx.Descriptors.textures.textures.push_back(&resources.defaultTexture);
		}
	}



	updateDescriptorSets(ctx);
}

void createDescriptorInfos(RendererContext& ctx)
{
	ctx.Descriptors.Descriptors =
	{
		&ctx.Descriptors.time,
		&ctx.Descriptors.camera,
		&ctx.Descriptors.shading,
		&ctx.Descriptors.textures
	};

	// textures
	ctx.Descriptors.textures.info = DescriptorInfo
	{
		.name = "Textures",
		.binding = 3,
		.set = 0,
		.count = VConfig::MAX_TEXTURES,
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.stages = VK_SHADER_STAGE_FRAGMENT_BIT
	};


	ctx.Descriptors.time.info = DescriptorInfo
	{
		.name = "Time",
		.binding = 0,
		.set = 0,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Descriptors.time.size = sizeof(TimeUBO);
	ctx.Descriptors.camera.info = DescriptorInfo
	{
		.name = "Camera",
		.binding = 1,
		.set = 0,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT
	};
	ctx.Descriptors.camera.size = sizeof(CameraUBO);
	ctx.Descriptors.shading.info = DescriptorInfo
	{
		.name = "Shading",
		.binding = 2,
		.set = 0,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Descriptors.shading.size = sizeof(ShadingUBO);

	// push constants
	ctx.Descriptors.pushConstants =
	{
		{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = 0,
			.size = sizeof(PushConstants)
		},

	};
}

void createDescriptorLayouts(RendererContext& ctx)
{
	uint32_t setCount = 0;
	for (const auto* desc : ctx.Descriptors.Descriptors) {
		setCount = std::max(setCount, desc->info.set + 1);
	}
	std::vector<VkDescriptorSetLayout> setLayouts(setCount);

	for (size_t i = 0; i < setCount; i++)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		for (const auto* uniform : ctx.Descriptors.Descriptors) {
			if (uniform->info.set != i) { continue; }
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = uniform->info.binding;
			binding.descriptorType = uniform->info.type;
			binding.descriptorCount = uniform->info.count;
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
	ctx.Descriptors.layouts = std::move(setLayouts);

}
void destroyDescriptorLayouts(RendererContext& ctx) {
	for(auto layout : ctx.Descriptors.layouts)
	vkDestroyDescriptorSetLayout(ctx.vulkanContext.device, layout, nullptr);
}
void createDescriptorPool(RendererContext& ctx) {

	VkDescriptorPoolSize poolSizes[] =
	{
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(ctx.Descriptors.Descriptors.size()) },
	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VConfig::MAX_TEXTURES },
	};

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 2;
	createInfo.pPoolSizes = poolSizes;
	createInfo.maxSets = static_cast<uint32_t>(ctx.Descriptors.layouts.size());

	VkDescriptorPool descPool;
	VK_CHECK(vkCreateDescriptorPool(ctx.vulkanContext.device, &createInfo, nullptr, &descPool), "ERROR: Failed to create descriptor pool");
	ctx.DescPool = descPool;

	// create imgui desc pool
	VkDescriptorPoolSize poolSizesIMGUI[] =
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
	ImGuicreateInfo.pPoolSizes = poolSizesIMGUI;

	VkDescriptorPool ImguidescPool;
	VK_CHECK(vkCreateDescriptorPool(ctx.vulkanContext.device, &ImGuicreateInfo, nullptr, &ImguidescPool), "ERROR: Failed to create ImGui descriptor pool");
	ctx.ImGuiDescPool = ImguidescPool;
}

void allocateDescriptorSets(RendererContext& ctx)
{


	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = ctx.DescPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(ctx.Descriptors.layouts.size());
	allocInfo.pSetLayouts = ctx.Descriptors.layouts.data();

	std::vector<VkDescriptorSet> sets(static_cast<uint32_t>(ctx.Descriptors.layouts.size()));
	VK_CHECK(vkAllocateDescriptorSets(ctx.vulkanContext.device, &allocInfo, sets.data()), "Error: Failed to allocate descriptor set");

	ctx.Descriptors.sets = std::move(sets);
}

void updateDescriptorSets(RendererContext& ctx) {

	std::vector<VkWriteDescriptorSet> writes;
	std::vector<VkDescriptorBufferInfo> bufferInfos;
	std::vector<VkDescriptorImageInfo> imageInfos;

	imageInfos.reserve(VConfig::MAX_TEXTURES);
	bufferInfos.reserve(ctx.Descriptors.Descriptors.size());
	writes.reserve(ctx.Descriptors.Descriptors.size());

	for (auto& Desc : ctx.Descriptors.Descriptors) {
		VkWriteDescriptorSet write{};

		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = ctx.Descriptors.sets[Desc->info.set];
		write.dstBinding = Desc->info.binding;
		write.descriptorCount = Desc->info.count;
		write.descriptorType = Desc->info.type;
		switch (Desc->info.type)
		{
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
			VkDescriptorBufferInfo bufferInfo{};

			bufferInfo.buffer = static_cast<UniformResources*>(Desc)->buffer.buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = static_cast<UniformResources*>(Desc)->size;

			bufferInfos.push_back(bufferInfo);
			write.pBufferInfo = &bufferInfos.back();
			break;
		}
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
			size_t imageStart = imageInfos.size();
			for (size_t i = 0; i < Desc->info.count; i++)
			{
				VkDescriptorImageInfo info{};
				info.sampler = static_cast<TextureResources*>(Desc)->textures[i]->sampler;
				info.imageView = static_cast<TextureResources*>(Desc)->textures[i]->image.view;
				info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfos.push_back(info);

			}
			write.pImageInfo = imageInfos.data() + imageStart;
			break;
		}
		default: {
			throw std::runtime_error("Error: Unknown Descriptor type!");
		}
		}
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
	for (auto* Desc : ctx.Descriptors.Descriptors) {
		if (Desc->info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
			static_cast<UniformResources*>(Desc)->buffer = createUniformBuffer(ctx.vulkanContext, static_cast<UniformResources*>(Desc)->size);
		}
	}
}
void destroyUniformBuffers(RendererContext& ctx)
{
	for (auto* Desc : ctx.Descriptors.Descriptors) {
		if (Desc->info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
			destroyBuffer(ctx.vulkanContext, static_cast<UniformResources*>(Desc)->buffer);
		}
	}
}

void bindDescriptorSets(RendererContext& ctx, VkCommandBuffer cmd) {

		vkCmdBindDescriptorSets(
			cmd,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			ctx.pipeline.pipelineLayout,
			0,
			ctx.Descriptors.sets.size(),
			ctx.Descriptors.sets.data(),
			0,
			nullptr
		);

}



void updatePerFrameUniformBuffers(RendererContext& RenderCtx, RenderData& Rdata)
{
	updateUniformBuffer<TimeUBO>(RenderCtx.vulkanContext, RenderCtx.Descriptors.time.buffer, Rdata.Uniforms.time);
	updateUniformBuffer<CameraUBO>(RenderCtx.vulkanContext, RenderCtx.Descriptors.camera.buffer, Rdata.Uniforms.camera);
	updateUniformBuffer<ShadingUBO>(RenderCtx.vulkanContext, RenderCtx.Descriptors.shading.buffer, Rdata.Uniforms.shading);
}
