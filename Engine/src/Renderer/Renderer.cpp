#include <Renderer.h>
#include <Window.h>
#include <Types/Scene.h>

#include <volk.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "Vulkan/VContext.h"
#include "RendererConfig.h"
#include "Vulkan/VCallbacks.h"

#include "Vulkan/VInstance.h"
#include "Vulkan/VDevice.h"
#include "Vulkan/VSwapchain.h"
#include "Vulkan/VCommands.h"
#include "Vulkan/VSync.h"
#include "Vulkan/VImage.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "MeshRenderer.h"
#include "Desciptors.h"

// datastructures
#include "../EngineData.h"
// memory
#include "Vulkan/MemoryHandler/memory.h"
#include "Vulkan/MemoryHandler/MemStructs.h"

#include "CmdRecording.h"
#include "FrameRenderer.h"
#include <memory>
#include "../Window/WindowInternal.h"
#include "../AssetManager/AssetTypes.h"
#include "ResourceUploader.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

struct Renderer::Impl {
	Object* selectedCamera = nullptr;
	RendererContext ctx;
	ResourceContext resources;
	RenderData data;
	Scene* loadedScene = nullptr;
	Window& window;
	Impl(Window& window)
		: window(window)
	{}
};

// Asset interface
TextureHandle Renderer::loadTexture(TextureData& texture) {
	Texture tex;
	populateTexture(vulkan->ctx, &tex, texture);
	TextureHandle handle{};
	vulkan->resources.textures.push_back(std::move(tex));
	handle.textureIndex = vulkan->resources.textures.size() - 1;
	handle.isValid = true;
	return handle;
}
MeshHandle Renderer::loadMesh(MeshData& meshData) {
	Mesh mesh;
	populateMesh(vulkan->ctx, &mesh, meshData.vertices, meshData.indices);
	MeshHandle handle{};
	mesh.layout = DefaultVertexLayouts::PositionNormalTexcoord();
	vulkan->resources.meshes.push_back(std::move(mesh));
	handle.meshIndex = vulkan->resources.meshes.size() - 1;
	handle.isValid = true;
	return handle;
}

Renderer::Renderer(Window& window)
	: vulkan(std::make_unique<Impl>(window)) {
	VulkanContext& ctx = vulkan->ctx.vulkanContext;
	volkInitialize();
	ctx.currentGraphicsState = VConfig::DefaultGraphicsState;

	createInstance(ctx);
	volkLoadInstance(ctx.instance);
	createDebugMessenger(ctx);
	createSurface(ctx, getGLFWWindow(window));
	pickPhysicalDevice(ctx);
	createLogicalDevice(ctx);
	volkLoadDevice(ctx.device);
	createSwapChain(ctx, getGLFWWindow(window));
	createCommandPool(ctx);
	createCommandBuffers(ctx);

	VkCommandBuffer tempcmd = beginSingleTimeCommands(ctx);
	transitionSwapchainDepthImage(ctx, tempcmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	endSingleTimeCommands(ctx, tempcmd);

	initializeSync(ctx);

	vulkan->resources.defaultTexture = createDefaultTexture(vulkan->ctx);
	
	


}



void Renderer::loadScene(Scene& scene) {
	setupDescriptorResources(vulkan->ctx, vulkan->resources, scene);
	initImGui();
	createShaders(vulkan->ctx);




	vulkan->loadedScene = &scene;
}
void Renderer::unloadScene(Scene& scene) {

}

void Renderer::Render(Scene& scene) {
	if (vulkan->selectedCamera == nullptr) {
		findFallbackCamera();
	}
	updateCameraUniforms();
	CreateEngineUI();
	DrawFrame(vulkan->ctx, vulkan->resources, getGLFWWindow(vulkan->window), scene, vulkan->data);
}
void Renderer::findFallbackCamera() {
		std::cout << "No camera selected, attempting to find a fallback";
		// no camera set, find first avaiable
		for (auto& obj : vulkan->loadedScene->objects) {
			if (std::find(obj.components.begin(), obj.components.end(), ObjectComponents::Camera) != obj.components.end()) {
				vulkan->selectedCamera = &obj;
				return;
			}
		}
		throw std::runtime_error("Error, no fallback camera found!");
	
}
void Renderer::setCamera(std::string name) {
	bool isFound = false;
	for (auto& obj : vulkan->loadedScene->objects) {
		if (obj.name == name && std::find(obj.components.begin(), obj.components.end(), ObjectComponents::Camera) != obj.components.end()) {
			vulkan->selectedCamera = &obj;
			isFound = true;
			break;
		}
	}
	if (!isFound) {
		std::cerr << "\nSet Camera: No camera with name " << name << " found!";
	}
}
void Renderer::updateCameraUniforms() {
	Object* cam = vulkan->selectedCamera;
	CameraUBO* ubo = &vulkan->data.Uniforms.camera;


	vulkan->data.Uniforms.shading.camerapos = glm::vec4(cam->position, 1);
	for (auto& obj : vulkan->loadedScene->objects) {
		if (std::find(obj.components.begin(), obj.components.end(), ObjectComponents::PointLight) != obj.components.end()) {
			vulkan->data.Uniforms.shading.lightpos = glm::vec4(obj.position, 1);

		}
	}

	ubo->projection = glm::perspective(glm::radians(45.0f), (16.0f/9.0f), 0.1f, 1000.0f);
	glm::mat4 rotation = glm::yawPitchRoll(
		cam->rotation.y,
		cam->rotation.x,
		cam->rotation.z
	);
	glm::vec3 forward = glm::vec3(rotation * glm::vec4(0, 0, 1, 0));
	glm::vec3 up = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
	ubo->view = glm::lookAt(
		cam->position,
		cam->position + forward,
		up
	);
	ubo->projection[1][1] *= -1;


}

Renderer::~Renderer() {
	vkDeviceWaitIdle(vulkan->ctx.vulkanContext.device);
	destroyUniformBuffers(vulkan->ctx);
	for (auto& mesh : vulkan->resources.meshes) {
		destroyBuffer(vulkan->ctx.vulkanContext, mesh.vertexBuffer);
		destroyBuffer(vulkan->ctx.vulkanContext, mesh.indexBuffer);
	}
	for (auto& texture : vulkan->resources.textures) {
		destroyImage(vulkan->ctx.vulkanContext, texture.image);
		vkDestroySampler(vulkan->ctx.vulkanContext.device, texture.sampler, nullptr);
	}
	destroyImage(vulkan->ctx.vulkanContext, vulkan->resources.defaultTexture.image);
	vkDestroySampler(vulkan->ctx.vulkanContext.device, vulkan->resources.defaultTexture.sampler, nullptr);

	ImGui_ImplVulkan_Shutdown();
	destroyShaders(vulkan->ctx);
	destroyDescriptorPool(vulkan->ctx);
	destroyPipelineLayout(vulkan->ctx);
	destroyDescriptorLayouts(vulkan->ctx);
	


	destroySync(vulkan->ctx.vulkanContext);
	destroyCommandPool(vulkan->ctx.vulkanContext);
	destroySwapchain(vulkan->ctx.vulkanContext);
	vkDestroyDevice(vulkan->ctx.vulkanContext.device, nullptr);
	vkDestroySurfaceKHR(vulkan->ctx.vulkanContext.instance, vulkan->ctx.vulkanContext.surface, nullptr);

	if (vulkan->ctx.vulkanContext.debugMessenger != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(
			vulkan->ctx.vulkanContext.instance,
			vulkan->ctx.vulkanContext.debugMessenger,
			nullptr
		);
	}

	vkDestroyInstance(vulkan->ctx.vulkanContext.instance, nullptr);
	std::cout << "\nVulkan successfully destroyed\n";
}


void Renderer::initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

	ImGui_ImplGlfw_InitForVulkan(getGLFWWindow(vulkan->window), true);
	ImGui_ImplVulkan_InitInfo info{};
	info.ApiVersion = VK_API_VERSION_1_4;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.

	VulkanContext& vctx = vulkan->ctx.vulkanContext;
	info.Instance = vctx.instance;
	info.PhysicalDevice = vctx.physicalDevice;
	info.Device = vctx.device;
	info.QueueFamily = vctx.queueContext.graphicsQueueFamily;
	info.Queue = vctx.queueContext.graphicsQueue;
	info.PipelineCache = VK_NULL_HANDLE;
	info.DescriptorPool = vulkan->ctx.ImGuiDescPool;
	info.MinImageCount = 2;
	info.ImageCount = vctx.swapchainContext.imageCount;
	info.Allocator = nullptr;
	info.UseDynamicRendering = true;
	info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	info.CheckVkResultFn = [](VkResult err)
	{
		if (err != VK_SUCCESS)
			std::cerr << "ImGui Vulkan error: " << err << "\n";
	};

	VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
	pipelineRenderingCreateInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

	pipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipelineRenderingCreateInfo.pColorAttachmentFormats =
		&vctx.swapchainContext.imageFormat;

	pipelineRenderingCreateInfo.depthAttachmentFormat = vctx.swapchainContext.depthFormat;


	info.PipelineInfoMain.PipelineRenderingCreateInfo =
		pipelineRenderingCreateInfo;


	ImGui_ImplVulkan_Init(&info);
}
void Renderer::CreateEngineUI() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	auto& shading = vulkan->data.Uniforms.shading;
	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	if (ImGui::BeginTabBar("MainTabs")) {

		if (ImGui::BeginTabItem("Shading")) {

			ImGui::ColorPicker4("Diffuse Color", &shading.diffuseColor.x);
			ImGui::ColorPicker4("Specular Color", &shading.spectralColor.x);
			ImGui::ColorPicker4("Ambient Color", &shading.ambientColor.x);

			ImGui::DragFloat("Specular Power", &shading.spectralPower, 1.0f, 10.0f, 100.0f);

			ImGuiCheckbox("Toggle Diffuse", shading.toggleDiffuse);
			ImGuiCheckbox("Toggle Specular", shading.toggleSpecular);
			ImGuiCheckbox("Toggle Ambient", shading.toggleAmbient);

			ImGuiCheckbox("Toggle Albido", shading.toggleAlbedo);
			ImGuiCheckbox("Toggle Normal", shading.toggleNormal);
			ImGuiCheckbox("Toggle Roughness", shading.toggleRoughness);


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera")) {
			ImGui::DragFloat3(
				"Camera Psoition",
				&shading.camerapos.x,
				0.05f
			);
			ImGui::DragFloat3(
				"Light Position",
				&shading.lightpos.x,
				0.05f
			);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Debug")) {
			ImGuiCheckbox("Display Normals", shading.debugNormals);


			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}





	ImGui::End();
	// Rendering
	ImGui::Render();
}
void Renderer::ImGuiCheckbox(const char* label, uint32_t& value)
{
	bool temp = value != 0;

	if (ImGui::Checkbox(label, &temp))
		value = temp ? 1u : 0u;
}

void Renderer::printRendererDebugInfo() {
	std::cout << "------------------\nRenderData:";
	std::cout << "Time counter: " << vulkan->data.Uniforms.time.FrameCount;
	std::cout << "Loaded textures: " << vulkan->resources.textures.size() << "\n";
	std::cout << "Loaded meshes: " << vulkan->resources.meshes.size() << "\n";
}









