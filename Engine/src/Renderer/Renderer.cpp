#include <Renderer/Renderer.h>
#include <Window/Window.h>
#include <Types/Scene.h>
#include "RendererInternal.h"

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
#include "ShaderData.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// Renderer functions
#include "Pipeline.h"
#include "Shaders.h"
#include "MeshRenderer.h"
#include "Desciptors.h"


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
	Scene* loadedScene = nullptr;

	RendererContext ctx;
	ResourceContext resources;

	Settings settings = {
		.renderSettings = VConfig::DefaultRenderSettings,
		.debugRenderSettings = VConfig::DefaultDebugRenderSettings
	};

	RenderData data;

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
MaterialHandle Renderer::loadMaterial(MaterialData& matData) {
	Material material{
		.baseColor = matData.baseColor,
		.specularColor = matData.specularColor,
		.specularPower = matData.specularPower,
		.metallic = matData.metallic,
		.roughness = matData.roughness,
		.ambientOcclusion = matData.ambientOcclusion,
	};
	if (matData.albedoTexture.isValid)
		material.albedoTexture = matData.albedoTexture.textureIndex;
	else
		material.albedoTexture = 0;

	if (matData.normalTexture.isValid)
		material.normalTexture = matData.normalTexture.textureIndex;
	else
		material.normalTexture = 1;

	if (matData.roughnessTexture.isValid)
		material.roughnessTexture = matData.roughnessTexture.textureIndex;
	else
		material.roughnessTexture = 2;

	if (matData.metallicTexture.isValid)
		material.metallicTexture = matData.metallicTexture.textureIndex;
	else
		material.metallicTexture = 3;

	if (matData.aoTexture.isValid)
		material.aoTexture = matData.aoTexture.textureIndex;
	else
		material.aoTexture = 4;

	MaterialHandle handle{};
	vulkan->data.StorageBuffers.materials.push_back(material);
	handle.materialIndex = vulkan->data.StorageBuffers.materials.size() - 1;
	handle.isValid = true;
	return handle;
}

Renderer::Renderer(Window& window)
	: vulkan(std::make_unique<Impl>(window)) {
	VulkanContext& ctx = vulkan->ctx.vulkanContext;
	volkInitialize();

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
	std::cout << "\nVulkan Init Done\n";


	// load default textures
	createDefaultTexture(vulkan->ctx, vulkan->resources.textures);
	// allocate render images

	


}



void Renderer::loadScene(Scene& scene) {
	std::cout << "\nRenderer: Loading scene:" << scene.name <<"\n";
	
	vulkan->loadedScene = &scene;
	initLightSSBO();
	initShadowMapRenderTargets(vulkan->ctx, vulkan->data, vulkan->resources);
	setupDescriptorResources(vulkan->ctx, vulkan->resources, scene, vulkan->data.StorageBuffers.materials.size());
	initImGui();
	createShaders(vulkan->ctx);
	std::cout << "\nRenderer: Loaded scene Sucessfully";

	
}
void Renderer::unloadScene() {
	std::cout << "\nRenderer: Unoading scene:" << vulkan->loadedScene->name << "\n";
}
void Renderer::beginImGuiFrame(bool drawEngineDebug) {
	if (vulkan->ctx.ImGUIEnabled) {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (drawEngineDebug) {
			CreateEngineUI();
		}
	}
}

void Renderer::Render(Scene& scene) {
	if (vulkan->selectedCamera == nullptr) {
		findFallbackCamera();
	}
	updatePerFrameData();
	DrawFrame(vulkan->ctx, vulkan->resources, getGLFWWindow(vulkan->window), scene, vulkan->data, vulkan->settings);
}
void Renderer::findFallbackCamera() {
		std::cout << "No camera selected, attempting to find a fallback";
		// no camera set, find first avaiable
		for (auto& obj : vulkan->loadedScene->objects) {
			if (obj.camera.has_value()) {
				vulkan->selectedCamera = &obj;
				std::cout << "Fallback camera found";
				return;
			}
		}
		throw std::runtime_error("Error, no fallback camera found!");
	
}
void Renderer::setCamera(std::string name) {
	bool isFound = false;
	for (auto& obj : vulkan->loadedScene->objects) {
		if (obj.name == name && obj.camera.has_value()) {
			vulkan->selectedCamera = &obj;
			isFound = true;
			break;
		}
	}
	if (!isFound) {
		std::cerr << "\nSet Camera: No camera with name " << name << " found!";
	}
}

void Renderer::initLightSSBO() {
	vulkan->data.StorageBuffers.lights.clear();
	vulkan->data.lightObjectIndices.clear();
	for (size_t i = 0; i < vulkan->loadedScene->objects.size(); ++i){
		auto& obj = vulkan->loadedScene->objects[i];
		if (obj.light.has_value()) {
			vulkan->data.lightObjectIndices.push_back(i);
			glm::mat4 rotationlight = glm::yawPitchRoll(
				obj.transform.rotation.y,
				obj.transform.rotation.x,
				obj.transform.rotation.z
			);
			glm::vec3 Lightforward = glm::normalize(rotationlight * glm::vec4(0, 0, 1, 0));
			glm::vec3 Lightup = glm::vec3(rotationlight * glm::vec4(0, 1, 0, 0));
			
			glm::mat4 projectionLight = glm::ortho(
				-10.0f, 10.0f,
				-10.0f, 10.0f,
				0.1f, 1000.0f
			);

			glm::mat4 viewLight = glm::lookAt(
				obj.transform.position,
				obj.transform.position + Lightforward,
				Lightup
			);
			glm::mat4 identity{ 1.0f };
			Light L{
				.type = +obj.light.value().type,
				.color = glm::vec4(obj.light.value().color, 1),
				
				.intensity = obj.light.value().intensity,
				.position = glm::vec4(obj.transform.position, 1),
				.direction = glm::vec4(Lightforward, 0),

				.range = obj.light.value().range,
				.spotInnerAngle = glm::radians(obj.light.value().spotInnerAngle),
				.spotOuterAngle = glm::radians(obj.light.value().spotOuterAngle),


				.castsShadow = obj.light.value().castsShadows,
				.shadowMapIndex = 0,
				.viewProj = identity,

			};
			vulkan->data.StorageBuffers.lights.push_back(L);

		}
	}
	vulkan->data.Uniforms.frame.lightCount = static_cast<uint32_t>(vulkan->data.StorageBuffers.lights.size());
	if(VConfig::PRINT_DEBUG_INFO){ std::cout << "\nScene has:" << vulkan->data.StorageBuffers.lights.size()  << " Lights\n"; }

}


void Renderer::updatePerFrameData() {

	// update light values
	for (size_t i = 0; i < vulkan->data.StorageBuffers.lights.size(); ++i) {
		auto& light = vulkan->data.StorageBuffers.lights[i];
		auto& obj = vulkan->loadedScene->objects[vulkan->data.lightObjectIndices[i]];

		 // pos
		light.type = +obj.light.value().type;
		light.position = glm::vec4(obj.transform.position, 1);
		light.color = glm::vec4(obj.light.value().color, 1);
		light.intensity = obj.light.value().intensity;

		light.range = obj.light.value().range;
		light.spotInnerAngle = obj.light.value().spotInnerAngle;
		light.spotOuterAngle = obj.light.value().spotOuterAngle;


		glm::mat4 rotationlight = glm::yawPitchRoll(
			glm::radians(obj.transform.rotation.y),
			glm::radians(obj.transform.rotation.x),
			glm::radians(obj.transform.rotation.z)
		);
		glm::vec3 Lightforward = glm::normalize(rotationlight * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		light.direction = glm::vec4(Lightforward, 0);

		glm::vec3 Lightup = glm::vec3(rotationlight * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

		glm::mat4 projectionLight = glm::orthoRH_ZO(
			-10.0f, 10.0f,
			-10.0f, 10.0f,
			0.01f, 1000.0f
		);
		glm::vec3 target = glm::vec3(0.0f);
		float shadowDistance = obj.light.value().shadowDistance;

		glm::vec3 shadowCameraPos =
			target - Lightforward * shadowDistance;

		glm::mat4 viewLight = glm::lookAt(
			shadowCameraPos,
			target,
			Lightup
		);
		
		light.viewProj = projectionLight * viewLight;

	}



	Object* cam = vulkan->selectedCamera;
	CameraUBO* ubo = &vulkan->data.Uniforms.camera;

	vulkan->data.Uniforms.camera.position = glm::vec4(cam->transform.position, 1);
	// update camera resources

	if (cam->camera.value().projection == Components::CameraProjection::Perspective) {
		ubo->projection = glm::perspective(cam->camera.value().fov, cam->camera.value().aspectRatio, cam->camera.value().nearPlane, cam->camera.value().farPlane);
	}
	else if (cam->camera.value().projection == Components::CameraProjection::Orthographic) {
		ubo->projection = glm::ortho(
			-10.0f, 10.0f,
			-10.0f, 10.0f,  
			0.1f, 1000.0f  
		);
	}
	else {
		ubo->projection = glm::perspective(cam->camera.value().fov, cam->camera.value().aspectRatio, cam->camera.value().nearPlane, cam->camera.value().farPlane);
	}
	glm::mat4 rotation = glm::yawPitchRoll(
		glm::radians(cam->transform.rotation.y),
		glm::radians(cam->transform.rotation.x),
		glm::radians(cam->transform.rotation.z)
	);
	glm::vec3 forward = glm::vec3(rotation * glm::vec4(0, 0, 1, 0));
	glm::vec3 up = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
	ubo->view = glm::lookAt(
		cam->transform.position,
		cam->transform.position + forward,
		up
	);
	ubo->projection[1][1] *= -1;



}

Settings& Renderer::getRenderSettings()
{
	return vulkan->settings;
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(vulkan->ctx.vulkanContext.device);
	destroyDescriptorBuffers(vulkan->ctx);
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
		&vctx.swapchainContext.colorTargets[0].format;

	pipelineRenderingCreateInfo.depthAttachmentFormat = vctx.swapchainContext.depthTargets[0].format;


	info.PipelineInfoMain.PipelineRenderingCreateInfo =
		pipelineRenderingCreateInfo;


	ImGui_ImplVulkan_Init(&info);
}
void Renderer::CreateEngineUI() {


	ImGui::Begin("Debug");                          // Create a window called "Hello, world!" and append into it.

	if (ImGui::BeginTabBar("MainTabs")) {

		if (ImGui::BeginTabItem("Debug")) {
			//ImGuiCheckbox("Display Normals", shading.debugNormals);
			ImGui::Checkbox("Display Wireframe", &vulkan->settings.debugRenderSettings.wireframe);
			if (vulkan->selectedCamera != nullptr) {
				ImGui::DragFloat3("Position", &vulkan->selectedCamera->transform.position.x, 0.1f);
				ImGui::DragFloat3("Rotation", &vulkan->selectedCamera->transform.rotation.x, 1.0f);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	// Rendering

}
void Renderer::ImGuiCheckbox(const char* label, uint32_t& value)
{
	bool temp = value != 0;

	if (ImGui::Checkbox(label, &temp))
		value = temp ? 1u : 0u;
}

void Renderer::printRendererDebugInfo() {
	std::cout << "------------------\nRenderData:";
	std::cout << "Time counter: " << vulkan->data.Uniforms.frame.FrameCount;
	std::cout << "Loaded textures: " << vulkan->resources.textures.size() << "\n";
	std::cout << "Loaded meshes: " << vulkan->resources.meshes.size() << "\n";
}









