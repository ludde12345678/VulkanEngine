#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GLFWwindow* createWindow();

void setupGLFWCallbacks(GLFWwindow* window);

void pollWindow(GLFWwindow* window);
