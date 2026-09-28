#include "Desciptors.h"
#include "RendererConfig.h"

void setupDescriptorResources(RendererContext& ctx, ResourceContext& resources, Scene& scene, uint32_t materialCount)
{
	// count lights
	uint32_t lightCount = 0;
	uint32_t ShadowCount = 0;
	for (auto& obj : scene.objects) {
		if (obj.light.has_value()) {
			lightCount++;
			if (obj.light.value().castsShadows) { ShadowCount++; }
		}
	}
	if (ShadowCount > VConfig::MAX_SHADOW_MAPS) { throw std::runtime_error("ERROR: Shadowmaps Exceed MAX"); }
	createDescriptorInfos(ctx, lightCount, materialCount);

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
			ctx.Descriptors.textures.textures.push_back(&resources.textures[0]); // default white
		}
	}



	updateDescriptorSets(ctx);

	if (VConfig::PRINT_DEBUG_INFO) {
		std::cout << "Descriptor Storage buffer sizes:" << "\n";
		std::cout << "Light count: " << lightCount << "\n";
		std::cout << "Light size: " << sizeof(Light) << "\n";
		std::cout << "Light buffer size: " << ctx.Descriptors.lights.size << "\n";

		std::cout << "Material count: " << materialCount << "\n";
		std::cout << "Material size: " << sizeof(Material) << "\n";
		std::cout << "Material buffer size: " << ctx.Descriptors.materials.size << "\n";
	}
}

void createDescriptorInfos(RendererContext& ctx, uint32_t lightCount, uint32_t materialCount)
{
	ctx.Descriptors.Descriptors =
	{
		&ctx.Descriptors.frame,
		&ctx.Descriptors.camera,
		&ctx.Descriptors.lights,
		&ctx.Descriptors.materials,
		&ctx.Descriptors.textures,
		&ctx.Descriptors.ShadowMaps
	};
	// global
#pragma region GLOBAL(SET 0)
	ctx.Descriptors.frame.info = DescriptorInfo
	{
		.name = "Frame",
		.binding = 0,
		.set = +DescSets::GLOBAL,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Descriptors.frame.size = sizeof(FrameUBO);

	ctx.Descriptors.camera.info = DescriptorInfo
	{
		.name = "Camera",
		.binding = 1,
		.set = +DescSets::GLOBAL,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Descriptors.camera.size = sizeof(CameraUBO);

	ctx.Descriptors.lights.info = DescriptorInfo
	{
		.name = "Lights",
		.binding = 2,
		.set = +DescSets::GLOBAL,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Descriptors.lights.size = lightCount * sizeof(Light);
#pragma endregion

#pragma region MATERIAL(SET 1)
	// material
	ctx.Descriptors.materials.info = DescriptorInfo
	{
		.name = "Materials",
		.binding = 0,
		.set = +DescSets::MATERIAL,
		.count = 1,
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.stages = VK_SHADER_STAGE_FRAGMENT_BIT
	};
	ctx.Descriptors.materials.size = materialCount * sizeof(Material);

	// textures
	ctx.Descriptors.textures.info = DescriptorInfo
	{
		.name = "Textures",
		.binding = 1,
		.set = +DescSets::MATERIAL,
		.count = VConfig::MAX_TEXTURES,
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.stages = VK_SHADER_STAGE_FRAGMENT_BIT,
		.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};
#pragma endregion

#pragma region SHADOW(SET 2)

	ctx.Descriptors.ShadowMaps.info = DescriptorInfo
	{
		.name = "Shadow Maps",
		.binding = 1,
		.set = +DescSets::SHADOW,
		.count = VConfig::MAX_SHADOW_MAPS,
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.stages = VK_SHADER_STAGE_FRAGMENT_BIT,
		.layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL
	};

#pragma endregion






	// push constants
	ctx.Descriptors.pushConstants =
	{
		{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = 0,
			.size = sizeof(PushConstants) + sizeof(uint32_t)
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

	// Count how many allocations are needed
	std::unordered_map<VkDescriptorType, uint32_t> count;
	for (const auto& desc : ctx.Descriptors.Descriptors) {
		count[desc->info.type] += desc->info.count;
	}
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (const auto& [type, count] : count) {
		poolSizes.push_back(
			{
				.type = type,
				.descriptorCount = count
			}
		);
	}

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	createInfo.pPoolSizes = poolSizes.data();
	createInfo.maxSets = static_cast<uint32_t>(ctx.Descriptors.layouts.size());

	VK_CHECK(vkCreateDescriptorPool(ctx.vulkanContext.device, &createInfo, nullptr, &ctx.DescPool), "ERROR: Failed to create descriptor pool");


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
	uint32_t ImageCount = 0;
	for (auto& Desc : ctx.Descriptors.Descriptors) {
		if (Desc->info.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
			ImageCount += Desc->info.count;
		}
	}

	imageInfos.reserve(ImageCount);
	bufferInfos.reserve(ctx.Descriptors.Descriptors.size());
	writes.reserve(ctx.Descriptors.Descriptors.size());

	for (auto& Desc : ctx.Descriptors.Descriptors) {
		if (Desc->info.count == 0) { continue; }
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

			bufferInfo.buffer = static_cast<BufferResource*>(Desc)->buffer.buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = static_cast<BufferResource*>(Desc)->size;

			bufferInfos.push_back(bufferInfo);
			write.pBufferInfo = &bufferInfos.back();
			break;
		}
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: {
			VkDescriptorBufferInfo bufferInfo{};

			bufferInfo.buffer = static_cast<BufferResource*>(Desc)->buffer.buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = static_cast<BufferResource*>(Desc)->size;

			bufferInfos.push_back(bufferInfo);
			write.pBufferInfo = &bufferInfos.back();
			break;
		}
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
			size_t imageStart = imageInfos.size();
			for (size_t i = 0; i < Desc->info.count; i++)
			{
				VkDescriptorImageInfo info{};
				TextureResources* texRe = static_cast<TextureResources*>(Desc);
				info.sampler = texRe->textures[i]->sampler;
				info.imageView = texRe->textures[i]->image.view;
				info.imageLayout = Desc->info.layout;
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
			static_cast<BufferResource*>(Desc)->buffer = createUniformBuffer(ctx.vulkanContext, static_cast<BufferResource*>(Desc)->size);
		}
		else if (Desc->info.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
			static_cast<BufferResource*>(Desc)->buffer = createStorageBuffer(ctx.vulkanContext, static_cast<BufferResource*>(Desc)->size);
		}
	}
}
void destroyDescriptorBuffers(RendererContext& ctx)
{
	for (auto* Desc : ctx.Descriptors.Descriptors) {
		if (Desc->info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || Desc->info.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
			destroyBuffer(ctx.vulkanContext, static_cast<BufferResource*>(Desc)->buffer);
		}
	}
}

void bindDescriptorSets(RendererContext& ctx, VkCommandBuffer cmd, DescSetsMask mask) {
	// add sets here
	if (hasSet(mask, DescSetsMask::GLOBAL)) {
		vkCmdBindDescriptorSets(
			cmd,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			ctx.pipeline.pipelineLayout,
			0,
			1,
			&ctx.Descriptors.sets.at(0),
			0,
			nullptr
		);
	}
	if (hasSet(mask, DescSetsMask::MATERIAL)) {
		vkCmdBindDescriptorSets(
			cmd,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			ctx.pipeline.pipelineLayout,
			1, // first set
			1, // count
			&ctx.Descriptors.sets.at(1), // sets
			0,
			nullptr
		);
	}
	if (hasSet(mask, DescSetsMask::SHADOW)) {
		vkCmdBindDescriptorSets(
			cmd,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			ctx.pipeline.pipelineLayout,
			2, // first set
			1, // count
			&ctx.Descriptors.sets.at(2), // sets
			0,
			nullptr
		);
	}

}



void updatePerFrameUniformBuffers(RendererContext& RenderCtx, RenderData& Rdata)
{
	// Update buffers with CPU data from Descriptors stuct
	updateBuffer<FrameUBO>(RenderCtx.vulkanContext, RenderCtx.Descriptors.frame.buffer, Rdata.Uniforms.frame);
	updateBuffer<CameraUBO>(RenderCtx.vulkanContext, RenderCtx.Descriptors.camera.buffer, Rdata.Uniforms.camera);
	// probably not good performance wise
	updateBuffer<Material>(RenderCtx.vulkanContext, RenderCtx.Descriptors.materials.buffer, Rdata.StorageBuffers.materials);
	updateBuffer<Light>(RenderCtx.vulkanContext, RenderCtx.Descriptors.lights.buffer, Rdata.StorageBuffers.lights);

}
