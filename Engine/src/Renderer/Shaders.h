#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "Vulkan/VContext.h"
#include "RendererConfig.h"
#include "Vulkan/VUtils.h"

#include "RendererContext.h"

void createShaders(RendererContext& ctx);

VkShaderCreateInfoEXT createShaderInfo(RendererContext& ctx, const std::vector<char>& code, std::string& name, VkShaderStageFlagBits stage, VkShaderStageFlags nextStage);

std::vector<char> readFile(const std::string filename);

void destroyShaders(RendererContext& ctx);

void bindShaders(RendererContext& ctx, VkCommandBuffer cmd, const std::string& name);
