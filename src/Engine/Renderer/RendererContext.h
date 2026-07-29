# pragma once

#include <glm/glm.hpp>
#include "../Scene/Scene.h"
#include "Vulkan/VContext.h"
#include "ShaderData.h"

struct UniformBuffers
{
	AllocatedBuffer timeUB;
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

struct DescriptorContext
{
	VkDescriptorPool pool = VK_NULL_HANDLE;
	uint32_t descriptorCount = 1;


	VkDescriptorSetLayout timeLayout = VK_NULL_HANDLE;
	VkDescriptorSet timeSet = VK_NULL_HANDLE;
};

struct RendererContext
{
	VulkanContext vulkanContext;

	UniformBuffers Uniforms;
	PipelineContext pipeline;
	ShaderContext shaders;
	DescriptorContext descriptors;

};