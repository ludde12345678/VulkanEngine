#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>

#include "VContext.h"
#include "VConfig.h"
#include "VUtils.h"

void createShaders(VulkanContext &ctx);

VkShaderCreateInfoEXT createShaderInfo(const std::vector<char>& code, VkShaderStageFlagBits stage);

std::vector<char> readFile(const std::string filename);

void destroyShaders(VulkanContext& ctx);

void createPipelineLayout(VulkanContext& ctx);

void destroyPipelineLayout(VulkanContext& ctx);