#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "WindowContext.h"

#include <volk.h>
#include <GLFW/glfw3.h>

//imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort




#include "GLFWCallbacks.h"

struct AppContext;



void initImgui(AppContext& appCtx);

void imGuiUpdate(AppContext& AppCtx);

void ImGuiCheckbox(const char* label, uint32_t& value);

void initWindow(AppContext& AppCtx);

void setupGLFWCallbacks(GLFWwindow* window);

void pollWindow(GLFWwindow* window);
