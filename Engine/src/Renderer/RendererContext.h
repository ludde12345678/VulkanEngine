# pragma once

#include "VResources.h"
#include "Vulkan/VContext.h"
#include "ShaderData.h"

// descriptors

struct DescriptorInfo {
	const char* name;
	uint32_t binding;
	uint32_t set;
	uint32_t count;

	VkDescriptorType type;
	VkShaderStageFlags stages;
};
struct DescriptorResource
{
	DescriptorInfo info;
};

struct TextureResources : DescriptorResource
{

	std::vector<Texture*> textures;

};

struct UniformResources : DescriptorResource
{
	uint32_t size;
	AllocatedBuffer buffer;

};

// add uniforms here:
struct DescriptorContext
{
	UniformResources time;
	UniformResources camera;
	UniformResources shading;
	// local
	UniformResources material;
	TextureResources textures;




	std::vector<DescriptorResource*> Descriptors;
	std::vector<VkDescriptorSetLayout> layouts;
	std::vector <VkDescriptorSet> sets;
	std::vector <VkPushConstantRange> pushConstants;

};


struct PipelineContext
{
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};

struct ShaderContext
{
	VkShaderEXT vertexShader = VK_NULL_HANDLE;
	VkShaderEXT fragmentShader = VK_NULL_HANDLE;
};

struct ResourceContext
{
	Texture defaultTexture;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;

};


struct RendererContext
{
	VulkanContext vulkanContext;

	DescriptorContext Descriptors;
	PipelineContext pipeline;
	ShaderContext shaders;

	VkDescriptorPool DescPool = VK_NULL_HANDLE;
	VkDescriptorPool ImGuiDescPool = VK_NULL_HANDLE;

	bool ImGUIEnabled = true;
};

