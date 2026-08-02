#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "GLFWConfig.h"
#include <iostream>
#include "../App/EngineData.h"

void error_callback(int error, const char* description);

void GLFW_FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height);


void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void GLFW_CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
