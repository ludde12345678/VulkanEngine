#pragma once

#include <volk.h>
#include <iostream>

#include "Vulkan/VContext.h"
#include "../Scene/Scene.h"
#include "Vulkan/VUtils.h"

void drawMesh(VkCommandBuffer cmd, const Mesh& mesh);

void SetAndbindVertexInputs(VkCommandBuffer cmd, const Mesh& mesh);
