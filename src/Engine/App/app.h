#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <volk.h>
#include <GLFW/glfw3.h>
#include "../Renderer/Renderer.h"
#include "../Window/Window.h"
#include "../Scene/Scene.h"
#include "EngineData.h"
#include "../Renderer/Desciptors.h"
#include "../AssetLoader/AssetLoader.h"
#include "appConfig.h"
#include "../Window/WindowContext.h"
#include "appContext.h"
#include "camera.h"
#include "input.h"
#include "../Scene/SceneManager.h"


// temp
#include "../Renderer/ResourceUploader.h"

class VulkanApp
{
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    AppContext AppCtx;


    void init();
    void mainLoop();
    void update();
    void render();
    void cleanup();
    void printDebugInfo();
    void DebugFunction();
};