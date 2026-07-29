#include <iostream>

#include "app.h"

VulkanApp::VulkanApp() {
	RenderCtx = RendererContext{};
	window = nullptr;
	renderData = RenderData{};
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
	RenderCtx.vulkanContext = InitializeVulkan(window);
	loadDebugScene();

	createDescriptorLayouts(RenderCtx);
	createDescriptorPool(RenderCtx);
	createPipelineLayout(RenderCtx);

	RenderCtx.Uniforms = AllocateUniformBuffers(RenderCtx.vulkanContext);

	allocateDescriptorSets(RenderCtx);
	updateDescriptorSets(RenderCtx);
	createShaders(RenderCtx);
}


void VulkanApp::mainLoop() {
	renderData.scene = &scene;
	while (!glfwWindowShouldClose(window)) {

		DrawFrame(RenderCtx, window, renderData);
		pollWindow(window);
		renderData.timeUniform.FrameCount += 1;
	}
}

void VulkanApp::cleanup() {
	vkDeviceWaitIdle(RenderCtx.vulkanContext.device);
	destroyBuffer(RenderCtx.vulkanContext, RenderCtx.Uniforms.timeUB);
	for (auto& mesh : scene.Meshes) {
		destroyBuffer(RenderCtx.vulkanContext, mesh.vertexBuffer);
	}

	destroyShaders(RenderCtx);
	destroyDescriptorPool(RenderCtx);
	destroyPipelineLayout(RenderCtx);
	destroyDescriptorLayouts(RenderCtx);
	DestroyVulkan(RenderCtx);
	
}

void VulkanApp::printRenderDataDebug() {
	std::cout << "------------------\nRenderData:";
	std::cout << "Time counter: " << renderData.timeUniform.FrameCount;
	std::cout << "ammount of meshes: " << renderData.scene->Meshes.size() << "\n";
}

void VulkanApp::loadDebugScene()
{

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
	triangle.vertexBuffer = createVertexBuffer<float>(RenderCtx.vulkanContext, vertices);
	triangle.vertexCount = 3;

	triangle2.layout = DefaultVertexLayouts::PositionColor();
	triangle2.vertexBuffer = createVertexBuffer<float>(RenderCtx.vulkanContext, vertices2);
	triangle2.vertexCount = 3;
	Scene testScene{};
	testScene.Meshes.push_back(std::move(triangle));
	testScene.Meshes.push_back(std::move(triangle2));
	scene = testScene;
}