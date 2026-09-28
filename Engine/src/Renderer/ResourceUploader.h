#pragma once

#include <glm/glm.hpp>
#include <volk.h>

#include "../Renderer/Vulkan/MemoryHandler/MemStructs.h"
#include "../Renderer/Vulkan/MemoryHandler/memory.h"
#include "../AssetManager/AssetTypes.h"
#include "RendererContext.h"

void createDefaultTexture(RendererContext& ctx, std::vector<Texture>& textures);

void populateMesh(RendererContext& ctx, Mesh* mesh, std::vector<Vertex> vert, std::vector<uint32_t> indices);

void populateTexture(RendererContext &ctx, Texture *texture, TextureData& texData);
