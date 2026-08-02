#pragma once

#include "Scene.h"
#include "../App/appContext.h"
#include "../AssetLoader/AssetLoader.h"
#include <glm/gtc/matrix_transform.hpp>


Scene createDebugScene(RendererContext& ctx);

Scene createTriangleScene(AppContext& ctx);
