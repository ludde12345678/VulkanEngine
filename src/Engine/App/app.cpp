#include <iostream>

#include "app.h"

VulkanApp::VulkanApp() {
	Vulkanctx = VulkanContext{};
	window = nullptr;
}
VulkanApp::~VulkanApp() {

}

void VulkanApp::run() {
	init();
	mainLoop();
	cleanup();
}

void VulkanApp::init() {
	glfwInit();
	window = createWindow();
	Vulkanctx = InitializeVulkan(window);
}

void VulkanApp::mainLoop() {
	while (!glfwWindowShouldClose(window)) {

		DrawFrame(Vulkanctx, window);
		pollWindow(window);
	}
}

void VulkanApp::cleanup() {
	DestroyVulkan(Vulkanctx);
}

