#pragma once

#include "CmdRecording.h"
#include <volk.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include "RenderStates.h"

void GeometryPass(VkCommandBuffer cmd, RendererContext& renderer, ResourceContext& resources, Scene& scene, RenderTarget& target, RenderTarget& depthTarget, const Settings& settings);

void DebugPass(VkCommandBuffer cmd, RendererContext& renderCtx, ResourceContext& resources, Scene& scene, RenderTarget& target, const Settings& settings);

void ShadowMapPass(VkCommandBuffer cmd, RendererContext& renderCtx, ResourceContext& resources, Scene& scene, RenderTarget& depthTarget, const Settings& settings);


