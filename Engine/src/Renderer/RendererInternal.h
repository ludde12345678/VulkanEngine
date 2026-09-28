#pragma once
#include "ShaderData.h"
#include "VResources.h"
struct RenderData {

	UBOs Uniforms;
	SSBOs StorageBuffers;

	// indices to access the obj from the light;
	std::vector<uint32_t> lightObjectIndices;

};
