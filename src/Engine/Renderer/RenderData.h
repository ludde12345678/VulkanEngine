# pragma once

#include <glm/glm.hpp>
#include "../Scene/Scene.h"
#include "Vulkan/VContext.h"
#include "ShaderData.h"

struct RenderData
{
	const Scene* scene;
	TimeUBO timeUniform;

};