#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include "RendererConfig.h"
#include "VResources.h"

// GPU Buffers
// descriptors
enum class DescSets : uint32_t
{
	GLOBAL = 0,
	MATERIAL = 1,
	SHADOW = 2

};
constexpr uint32_t operator+(DescSets c) noexcept {
	return static_cast<uint32_t>(c);
}
enum class DescSetsMask : uint32_t
{
	NONE = 0,
	GLOBAL = 1 << +DescSets::GLOBAL, // set 0
	MATERIAL = 1 << +DescSets::MATERIAL, // set 1
	SHADOW = 1 << +DescSets::SHADOW, // set 2

};
constexpr DescSetsMask operator|(DescSetsMask a, DescSetsMask b)
{
	return static_cast<DescSetsMask>(
		static_cast<uint32_t>(a) |
		static_cast<uint32_t>(b)
		);
}

constexpr bool hasSet(DescSetsMask mask, DescSetsMask set)
{
	return (static_cast<uint32_t>(mask) &
		static_cast<uint32_t>(set)) != 0;
}

struct DescriptorInfo {
	const char* name;
	uint32_t binding;
	uint32_t set;
	uint32_t count;

	VkDescriptorType type;
	VkShaderStageFlags stages;
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};
struct DescriptorResource
{
	DescriptorInfo info;
};

struct TextureResources : DescriptorResource
{

	std::vector<Texture*> textures;

};


struct BufferResource : DescriptorResource
{
	uint32_t size;
	AllocatedBuffer buffer;

};
// Final Struct
// add uniforms here:
struct DescriptorContext
{
	// GLOBAL
	BufferResource frame;
	BufferResource camera;
	BufferResource lights;
	// MATERIAL
	BufferResource materials;
	TextureResources textures;
	// SHADOWS
	TextureResources ShadowMaps;



	std::vector<DescriptorResource*> Descriptors;
	std::vector<VkDescriptorSetLayout> layouts;
	std::vector <VkDescriptorSet> sets;
	std::vector <VkPushConstantRange> pushConstants;

};
// CPU side structs

// SET 0, GLOBAL
#pragma region GLOBAL
struct FrameUBO {
	uint32_t FrameCount = 0;
	float deltaTime;
	float totalTime;
	uint32_t lightCount = 0;
};
// SET 0, GLOBAL
struct CameraUBO
{
	glm::mat4 view = glm::mat4(1);
	glm::mat4 projection = glm::mat4(1);
	glm::vec4 position = glm::vec4(0, -5, 5, 1);
};
// SET 0, GLOBAL
struct alignas(16) Light
{
	uint32_t type;

	alignas(16) glm::vec4 color;

	float intensity;
	uint32_t _padding0[3];

	alignas(16) glm::vec4 position;
	alignas(16) glm::vec4 direction;

	float range;
	float spotInnerAngle;
	float spotOuterAngle;
	uint32_t castsShadow;
	uint32_t shadowMapIndex;

	alignas(16) glm::mat4 viewProj;

	uint32_t _padding1[2];
};
static_assert(sizeof(Light) == 192);
static_assert(offsetof(Light, color) == 16);
static_assert(offsetof(Light, position) == 48);
static_assert(offsetof(Light, direction) == 64);
static_assert(offsetof(Light, viewProj) == 112);
#pragma endregion

// SET 1, MATERIAL
struct alignas(16) Material
{
	// Base surface properties
	glm::vec4 baseColor;
	glm::vec4 specularColor;

	// Blinn-Phong / PBR parameters
	float specularPower;
	float metallic;
	float roughness;
	float ambientOcclusion;

	// Texture indices
	uint32_t albedoTexture;
	uint32_t normalTexture;
	uint32_t roughnessTexture;
	uint32_t metallicTexture;

	uint32_t aoTexture;
	uint32_t _padding[3];
};


// add shadow UBOs here if needed





struct PushConstants
{
	glm::mat4 modelMatrix;
	int32_t materialIndex = 99;
	float padding;
};
constexpr uint32_t PUSH_CONSTANT_SHADOW_OFFSET = sizeof(PushConstants);
constexpr uint32_t PUSH_CONSTANT_TOTAL_SIZE = sizeof(PushConstants) + sizeof(uint32_t);
// Main Shader Data container
struct UBOs {
	FrameUBO frame;
	CameraUBO camera;
};
struct SSBOs {
	std::vector<Material> materials;
	std::vector<Light> lights;
};