
#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "../Renderer/VertexLayouts.h"
#include <Types/Handles.h>


#include <glm/glm.hpp>


struct MeshData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};


struct TextureData
{
    std::string name;

    uint32_t width;
    uint32_t height;

    std::vector<uint8_t> pixels;

};
struct MaterialData
{
	glm::vec4 baseColor = glm::vec4(1.0f);
	glm::vec4 specularColor = glm::vec4(1.0f);

	// Blinn-Phong / PBR parameters
	float specularPower = 0.0f;
	float metallic = 0.0f;
	float roughness = 0.0f;
	float ambientOcclusion = 0.0f;

	// Texture indices
	TextureHandle albedoTexture;
	TextureHandle normalTexture;
	TextureHandle roughnessTexture;
	TextureHandle metallicTexture;

	TextureHandle aoTexture;
};

struct AudioClip
{
    std::string name;

    std::vector<uint8_t> data;

    uint32_t sampleRate;
    uint32_t channels;
};