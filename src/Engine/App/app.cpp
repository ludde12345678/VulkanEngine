#include <iostream>

#include "app.h"

VulkanApp::VulkanApp() {
	AppCtx = AppContext{};
}
VulkanApp::~VulkanApp() {

}

void VulkanApp::run() {
	init();
	mainLoop();
	cleanup();
}

void VulkanApp::init() {
	// GLFW
	initWindow(AppCtx);

	InitializeVulkan(AppCtx.renderer, AppCtx.window.window);

	if (appConfig::enableImGui) {
		initImgui(AppCtx);
	}

	AppCtx.scene = createDebugScene(AppCtx.renderer);
	//AppCtx.scene = loadMeshScene(AppCtx.renderer, "utah_teapot.obj");
	if (appConfig::runDebugFunction) { DebugFunction(); return; }


	
}
void VulkanApp::mainLoop() {
	while (!glfwWindowShouldClose(AppCtx.window.window)) {
		if (appConfig::enableImGui) {
			imGuiUpdate(AppCtx);
		}
		
		update();
		updateInputs(AppCtx.frame);
		updateCameraUniforms(AppCtx.frame);
		render();
		pollWindow(AppCtx.window.window);
	}
}

void VulkanApp::update() {
	auto& camera = AppCtx.frame.Uniforms.camera;
	auto& shading = AppCtx.frame.Uniforms.shading;

	glm::vec3 cameraPos = shading.camerapos;

	// Looking forward along -Z
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	camera.view = glm::lookAt(
		cameraPos,
		cameraTarget,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	float aspect =
		static_cast<float>(AppCtx.renderer.vulkanContext.swapchainContext.extent.width) /
		static_cast<float>(AppCtx.renderer.vulkanContext.swapchainContext.extent.height);

	camera.projection = glm::perspective(
		glm::radians(60.0f),
		aspect,
		0.1f,
		100.0f
	);


	camera.projection[1][1] *= -1;
}
void VulkanApp::render() {
	DrawFrame(AppCtx.renderer, AppCtx.window.window, AppCtx.scene, AppCtx.frame);
}

void VulkanApp::cleanup() {

	DestroyVulkan(AppCtx.renderer, AppCtx.scene);
	
}

void VulkanApp::printDebugInfo() {

}

void VulkanApp::DebugFunction()
{
	//loadMeshScene("utah_teapot.obj");

}