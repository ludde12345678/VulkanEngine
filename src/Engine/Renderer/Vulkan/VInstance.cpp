#include <string.h>
#include "VInstance.h"



void createInstance(VulkanContext& ctx)
{
	VkInstance instance = VK_NULL_HANDLE;

	// app info
	VkApplicationInfo appinfo{};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "Vulkan Engine";
	appinfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appinfo.pEngineName = "No Engine";
	appinfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appinfo.apiVersion = VK_API_VERSION_1_3;

	// check if in debug

	if (VConfig::ENABLE_VALIDATION && !checkValidationLayerSupport(VConfig::validationLayers)) 
	{ 
		throw std::runtime_error("Validation Layer not available"); 
	}


	// Extensions
	std::vector<const char*> extensions = VConfig::instanceExtensions;
	uint32_t glfwExCount = 0;
	const char** glfwEx = glfwGetRequiredInstanceExtensions(&glfwExCount);
	if (glfwEx == nullptr){throw std::runtime_error("Failed to get GLFW Vulkan extensions");}
	extensions.insert(
		extensions.end(),
		glfwEx,
		glfwEx + glfwExCount
	);

	// add additional Extensions here
	if (VConfig::ENABLE_VALIDATION) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!checkExtensionSupport(extensions)){ throw std::runtime_error("Extension not available"); }

	// debug messenger
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	// vulcan instance create info
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appinfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());;
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (VConfig::ENABLE_VALIDATION) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(VConfig::validationLayers.size());
		createInfo.ppEnabledLayerNames = VConfig::validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// create instance
	VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);

	// check if successful
	if (res != VK_SUCCESS) {
		std::cout << "\n" << "ERROR: Vulkan Init";
		throw std::runtime_error("Vulkan instance creation failed");
	}
	std::cout << "\n" << "VulkanSuccess";
	// print debug info
	
	if (VConfig::PRINT_DEBUG_INFO) {
		std::cout << "\nEnabled Extensions:\n";
		for (const auto& ext : extensions)
		{
			std::cout << "\t" << ext << "\n";
		}

		std::cout << "\nEnabled Validation Layers:\n";
		for (const auto& layer : VConfig::validationLayers)
		{
			std::cout << "\t" << layer << "\n";
		}
	}
	


	ctx.instance = instance;
}

void createSurface(VulkanContext& ctx, GLFWwindow* window) {
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(ctx.instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Window surface creation failed!");
	}

	ctx.surface = surface;

}

bool checkExtensionSupport(const std::vector<const char*>& RequestedExtensions) {
	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, availableExtensions.data());

	for (const auto& Extension : RequestedExtensions) {
		bool found = false;
		for (const auto& existingExtensions : availableExtensions) {
			if (strcmp(existingExtensions.extensionName, Extension) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}

	}
	return true;
}
bool checkValidationLayerSupport(const std::vector<const char*>& RequestedLayers) {
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const auto& layer : RequestedLayers) {
		bool found = false;
		for (const auto& existingLayers : availableLayers) {
			if (strcmp(existingLayers.layerName, layer) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
		
	}
	return true;
}

void createDebugMessenger(VulkanContext& ctx) {
	if (!VConfig::ENABLE_VALIDATION) {
		ctx.debugMessenger = VK_NULL_HANDLE;
		return;
	}
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	if (vkCreateDebugUtilsMessengerEXT(ctx.instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
	ctx.debugMessenger = debugMessenger;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}


