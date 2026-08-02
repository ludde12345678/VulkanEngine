#pragma once

#include "../Renderer/RendererContext.h"
#include "../Window/WindowContext.h"
#include "EngineData.h"

struct AppContext
{
    // Contexts
    RendererContext renderer = RendererContext{};
    WindowContext window = WindowContext{};

    // Resources
    RenderData frame = RenderData{};
    Scene scene = Scene{};

};