#pragma once

#include <Types/Scene.h>

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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "MeshRenderer.h"
#include "Desciptors.h"
#include "CmdRecording.h"

// datastructures
#include "../EngineData.h"
// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"




void DrawFrame(RendererContext& RenderCtx, ResourceContext& resources, GLFWwindow* window, Scene& scene, RenderData& Rdata);


void recreateSwapchainResources(VulkanContext& ctx, GLFWwindow* window);

void waitForFrame(VulkanContext& ctx);

uint32_t getSwapchainImage(VulkanContext& ctx);

void submitFrame(VulkanContext& ctx, uint32_t imageIndex);

void presentFrame(VulkanContext& ctx, uint32_t imageIndex);