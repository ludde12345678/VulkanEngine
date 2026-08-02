#pragma once

#include <glm/glm.hpp>
#include <volk.h>

#include "../Renderer/Vulkan/MemoryHandler/MemStructs.h"
#include "../Renderer/Vulkan/MemoryHandler/memory.h"
#include "../Scene/Scene.h"
#include "RendererContext.h"

void populateMesh(RendererContext& ctx, Mesh* mesh, std::vector<Vertex> vert, std::vector<uint32_t> indices);
