#pragma once




#include <volk.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "Vulkan/VContext.h"
#include "RendererConfig.h"
#include "Vulkan/VCallbacks.h"

#include "Vulkan/VInstance.h"
#include "Vulkan/VDevice.h"
#include "Vulkan/VSwapchain.h"
#include "Vulkan/VCommands.h"
#include "Vulkan/VSync.h"
#include "Vulkan/VImage.h"

#include <imgui_impl_vulkan.h>

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "MeshRenderer.h"
#include "Desciptors.h"

// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"

#include "CmdRecording.h"
#include "FrameRenderer.h"

#include "Vulkan/VContext.h"

#include "Vulkan/VUtils.h"

void drawMesh(VkCommandBuffer cmd, const Mesh& mesh);

void SetAndbindVertexInputs(VkCommandBuffer cmd, const Mesh& mesh);
