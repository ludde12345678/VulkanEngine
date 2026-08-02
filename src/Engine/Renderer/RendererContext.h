# pragma once

#include <glm/glm.hpp>
#include "../Scene/Scene.h"
#include "Vulkan/VContext.h"
#include "ShaderData.h"

struct UniformInfo {
	const char* name;
	uint32_t binding;
	uint32_t set;
	uint32_t size;

	VkDescriptorType type;
	VkShaderStageFlags stages;
};

struct UniformResources
{

	UniformInfo info;

	AllocatedBuffer buffer;

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


// add uniforms here:
struct UniformContext
{
	UniformResources time;
	UniformResources camera;
	UniformResources shading;

	std::vector<UniformResources*> all;

	std::vector<VkDescriptorSetLayout> layouts;
	std::vector <VkDescriptorSet> sets;

	std::vector <VkPushConstantRange> pushConstants;

};


struct RendererContext
{
	VulkanContext vulkanContext;

	UniformContext Uniforms;
	PipelineContext pipeline;
	ShaderContext shaders;

	VkDescriptorPool DescPool = VK_NULL_HANDLE;
	VkDescriptorPool ImGuiDescPool = VK_NULL_HANDLE;

};

