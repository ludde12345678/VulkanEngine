#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Vulkan/VContext.h"
#include "Vulkan/VConfig.h"
#include "Vulkan/VUtils.h"

#include "RendererContext.h"

void createShaders(RendererContext& ctx);

VkShaderCreateInfoEXT createShaderInfo(RendererContext& ctx, const std::vector<char>& code, VkShaderStageFlagBits stage, VkShaderStageFlags nextStage);

std::vector<char> readFile(const std::string filename);

void destroyShaders(RendererContext& ctx);

void bindShaders(RendererContext& ctx, VkCommandBuffer cmd);
