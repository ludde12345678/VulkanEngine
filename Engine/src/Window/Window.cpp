#include <Renderer.h>

#include <glm/glm.hpp>
#include <iostream>
#include <Window.h>
#include "WindowInternal.h"

#include <volk.h>
#include <GLFW/glfw3.h>

//imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort

#include "GLFWCallbacks.h"
#include "../Renderer/RendererInternal.h"
#include <Window.h>

struct Window::Impl {
    GLFWwindow* window;
    ImGuiIO* ImguiIO;
};

bool Window::shouldClose()
{
    return glfwWindowShouldClose(context->window);
}

Window::Window(int width, int height, const char* title)
    : context(std::make_unique<Impl>()) {
    glfwInit();
    if (!glfwInit())
    {
        std::cout << "GLFW failed\n";

    }

    // GLFW Error callback
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create window
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(monitor); // Valid on GLFW 3.3+ only
    if (GLFWConfig::FULLSCREEN) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        context->window = glfwCreateWindow(mode->width, mode->height, title, glfwGetPrimaryMonitor(), NULL);
    }
    else
    {
        context->window = glfwCreateWindow((int)(2560 * main_scale), (int)(1440 * main_scale), "Vulkan Renderer", nullptr, nullptr);
    }
    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("GLFW: Vulkan Not Supported\n");
    }
}
void Window::disableCursor(bool disabled)
{
    if (disabled) {
        glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}


GLFWwindow* getGLFWWindow(Window& window)
{
    return window.context->window;
}

void Window::setupGLFWCallbacks(Input& input) {

    glfwSetWindowUserPointer(context->window, &input);

    glfwSetKeyCallback(context->window, GLFW_KeyCallback);
    glfwSetCursorPosCallback(context->window, GLFW_CursorPosCallback);
    glfwSetMouseButtonCallback(context->window, GLFW_MouseButtonCallback);
}
void Window::pollWindow() {
    glfwPollEvents();
}

Window::~Window() = default;