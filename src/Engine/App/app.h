#pragma once

#include "../Renderer/Renderer.h"
#include "../Window/Window.h"
#include "../Scene/Scene.h"
#include "../Renderer/RenderData.h"
#include "../Renderer/Desciptors.h"
// temp
#include "../Renderer/ResourceUploader.h"

class VulkanApp
{
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    GLFWwindow* window;
    RendererContext RenderCtx;
    RenderData renderData;
    Scene scene;



    void init();
    void loadDebugScene();
    void mainLoop();
    void cleanup();
    void printRenderDataDebug();
};