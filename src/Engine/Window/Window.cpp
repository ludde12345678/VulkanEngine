#include <iostream>

#include "Window.h"
#include "GLFWCallbacks.h"



GLFWwindow* createWindow() {
    if (!glfwInit())
    {
        std::cout << "GLFW failed\n";
        
    }

    // GLFW Error callback
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // create window
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "LE window", NULL, NULL);
    

    return window;

}
void setupGLFWCallbacks(GLFWwindow* window) {

    glfwSetFramebufferSizeCallback(window, GLFW_FramebufferSizeChangedCallback);
}
void pollWindow(GLFWwindow* window) {
    glfwPollEvents();
}