#pragma once

#include "../Renderer/Renderer.h"
#include "../Window/Window.h"
#include "../Scene/Scene.h"

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
    VulkanContext Vulkanctx;
    Scene scene;



    void init();
    void mainLoop();
    void cleanup();
};