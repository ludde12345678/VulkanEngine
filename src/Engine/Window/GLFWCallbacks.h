#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void GLFW_FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height) {

}