# pragma once

#include "VResources.h"
#include "Vulkan/VContext.h"
#include "ShaderData.h"
#include <utility>





struct PipelineContext
{
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};

struct ShaderContext
{
	std::unordered_map<std::string, VkShaderEXT> vertexShaders;
	std::unordered_map<std::string, VkShaderEXT> fragmentShaders;
	std::unordered_map<std::string, VkShaderEXT> computeShaders;
};

struct ResourceContext
{
	Texture defaultTexture;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::vector<Texture> ShadowMapTextures;

};
struct RenderTargets
{
	std::vector <RenderTarget> shadowMaps;

	std::vector<AllocatedImage> images;
};
struct RendererContext
{
	VulkanContext vulkanContext;

	DescriptorContext Descriptors;
	PipelineContext pipeline;
	ShaderContext shaders;
	RenderTargets targets;

	VkDescriptorPool DescPool = VK_NULL_HANDLE;
	VkDescriptorPool ImGuiDescPool = VK_NULL_HANDLE;

	bool ImGUIEnabled = true;
};

