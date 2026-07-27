#pragma once

#include <volk.h>
#include <iostream>

#include "VContext.h"
#include "../../Scene/Scene.h"
#include "VUtils.h"

void drawMesh(VkCommandBuffer cmd, const Mesh& mesh);

void SetAndbindVertexInputs(VkCommandBuffer cmd, const Mesh& mesh);
