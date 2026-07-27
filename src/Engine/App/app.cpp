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
	setupGLFWCallbacks(window);
	Vulkanctx = InitializeVulkan(window);
	Mesh triangle{};
	Mesh triangle2{};
	std::vector<float> vertices =
	{
		// position          // color
	   -0.75f, -0.433f, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f,
	   -0.25f, -0.433f, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f,
	   -0.50f,  0.433f, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f
	};
	std::vector<float> vertices2 =
	{
		// position          // color
		0.25f, -0.433f, 0.0f,   0.0f, 1.0f, 1.0f, 1.0f,
		0.75f, -0.433f, 0.0f,   0.0f, 1.0f, 1.0f, 1.0f,
		0.50f,  0.433f, 0.0f,   0.0f, 1.0f, 1.0f, 1.0f
	};
	triangle.layout = DefaultVertexLayouts::PositionColor();
	triangle.vertexBuffer = createVertexBuffer<float>(Vulkanctx, vertices);
	triangle.vertexCount = 3;

	triangle2.layout = DefaultVertexLayouts::PositionColor();
	triangle2.vertexBuffer = createVertexBuffer<float>(Vulkanctx, vertices2);
	triangle2.vertexCount = 3;
	Scene testScene{};
	testScene.Meshes.push_back(std::move(triangle));
	testScene.Meshes.push_back(std::move(triangle2));

	scene = testScene;

}

void VulkanApp::mainLoop() {
	std::cout << "ammount of meshes: " << scene.Meshes.size() << "\n";
	while (!glfwWindowShouldClose(window)) {

		DrawFrame(Vulkanctx, window, scene);
		pollWindow(window);
	}
}

void VulkanApp::cleanup() {
	vkDeviceWaitIdle(Vulkanctx.device);
	for (auto& mesh : scene.Meshes) {
		destroyBuffer(Vulkanctx, mesh.vertexBuffer);
	}


	DestroyVulkan(Vulkanctx);
	
}

