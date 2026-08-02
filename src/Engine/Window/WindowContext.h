#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>

struct WindowContext
{
	GLFWwindow* window;
	ImGuiIO* ImguiIO;
};