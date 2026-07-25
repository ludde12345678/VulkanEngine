#pragma once

#include "../Renderer/Renderer.h"
#include "../Window/Window.h"

class VulkanApp
{
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    GLFWwindow* window;
    VulkanContext Vulkanctx;



    void init();
    void mainLoop();
    void cleanup();
};