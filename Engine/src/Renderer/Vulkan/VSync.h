#pragma once

#include <volk.h>
#include <iostream>

#include "VContext.h"
#include "../RendererConfig.h"
#include "VUtils.h"


void initializeSync(VulkanContext& ctx);

void destroySync(VulkanContext& ctx);

void recreateImageSync(VulkanContext& ctx);