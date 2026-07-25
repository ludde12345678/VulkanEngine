
#include "VDevice.h"


void pickPhysicalDevice(VulkanContext& ctx) {
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	uint32_t devcount = 0;
	vkEnumeratePhysicalDevices(ctx.instance, &devcount, nullptr);
	if (devcount == 0) { throw std::runtime_error("No devices with vulkan support"); }
	std::vector<VkPhysicalDevice> devices(devcount);
	vkEnumeratePhysicalDevices(ctx.instance, &devcount, devices.data());

	// iterate over devices
	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device, ctx.surface)) {
			physicalDevice = device;
			break;
		}

	}
	if (physicalDevice == VK_NULL_HANDLE) { throw std::runtime_error("No suitable device found"); }
	// log selected device if in debug
	if (VConfig::PRINT_DEBUG_INFO) {
		logPhysicalDevice(physicalDevice);
	}

	ctx.physicalDevice = physicalDevice;
}

void createLogicalDevice(VulkanContext& ctx) {
	QueueFamilies queueFamilies = findQueueFamilies(ctx.physicalDevice, ctx.surface);
	
	// Device Features
	VkPhysicalDeviceFeatures deviceFeatures{};

	VkPhysicalDeviceSynchronization2Features sync2Features{};
	sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	sync2Features.pNext = nullptr;
	sync2Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjFeatures{};
	shaderObjFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
	shaderObjFeatures.shaderObject = VK_TRUE;
	shaderObjFeatures.pNext = &sync2Features;

	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	dynamicRenderingFeatures.pNext = &shaderObjFeatures;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;


	// TODO:Expand this

	// queues
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { queueFamilies.graphicsFamily.value(), 
		queueFamilies.presentFamily.value() };
	float queuePriority = 1.0f;

	for (uint32_t queueFam : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFam;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}



	//Logical Device
	VkDeviceCreateInfo devicecreateInfo{};
	devicecreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devicecreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	devicecreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	devicecreateInfo.pEnabledFeatures = &deviceFeatures;
	devicecreateInfo.pNext = &dynamicRenderingFeatures;
	devicecreateInfo.enabledExtensionCount = static_cast<uint32_t>(VConfig::deviceExtensions.size());
	devicecreateInfo.ppEnabledExtensionNames = VConfig::deviceExtensions.data();
	devicecreateInfo.enabledLayerCount = 0;

	// create logicalDevice
	VkDevice logicalDevice;
	if (vkCreateDevice(ctx.physicalDevice, &devicecreateInfo, nullptr, &logicalDevice)!= VK_SUCCESS) {
		throw std::runtime_error("Error creating logical device");
	}
	VkQueue graphicsQueue;
	vkGetDeviceQueue(logicalDevice, queueFamilies.graphicsFamily.value(), 0, &graphicsQueue);

	VkQueue presentQueue;
	vkGetDeviceQueue(logicalDevice, queueFamilies.presentFamily.value(), 0, &presentQueue);




	QueueContext logicalContext{};

	ctx.device = logicalDevice;
	logicalContext.graphicsQueue = graphicsQueue;
	logicalContext.presentQueue = presentQueue;
	logicalContext.graphicsQueueFamily = queueFamilies.graphicsFamily.value();
	logicalContext.presentQueueFamily = queueFamilies.presentFamily.value();

	ctx.queueContext = logicalContext;

	if (VConfig::PRINT_DEBUG_INFO) {
		logLogicalDevice(ctx);
	}


}
// TODO: make this actually check the reuqirements!
bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
	VConfig::DeviceRequirements req;

	// get some info on the device
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	QueueFamilies families = findQueueFamilies(device, surface);

	bool extensionsSupported = checkDeviceExtensionSupport(device, VConfig::deviceExtensions);
	
	return (deviceFeatures.geometryShader == req.requireGeometryShader) &&
		families.hasRequiredQueues() &&
		checkDeviceFeatures(device) &&
		extensionsSupported;
}
bool checkDeviceFeatures(VkPhysicalDevice device)
{
	VkPhysicalDeviceFeatures2 features{};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;


	VkPhysicalDeviceShaderObjectFeaturesEXT shaderObject{};
	shaderObject.sType =
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;


	VkPhysicalDeviceDynamicRenderingFeatures dynamicRendering{};
	dynamicRendering.sType =
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;


	shaderObject.pNext = &dynamicRendering;
	features.pNext = &shaderObject;


	vkGetPhysicalDeviceFeatures2(device, &features);


	return shaderObject.shaderObject &&
		dynamicRendering.dynamicRendering;
}

bool checkDeviceExtensionSupport( VkPhysicalDevice physicalDevice, const std::vector<const char*>& RequestedExtensions) {
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice,nullptr, &extCount, availableExtensions.data());

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

QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
	QueueFamilies families;
	
	uint32_t famCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &famCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(famCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &famCount, queueFamilies.data());
	int i = 0;
	for (const auto& family : queueFamilies) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			families.graphicsFamily = i;
		}
		if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
			families.transferFamily = i;
		}
		if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			families.computeFamily = i;
		}
		if (presentSupport) {
			families.presentFamily = i;
		}
		if (families.hasRequiredQueues()) {
			break;
		}

		i++;
	}


	return families;
}


void logPhysicalDevice(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device, &properties);
	std::cout << "-------------------------------------" << '\n';
	std::cout << "Physical device information: " << '\n';
	std::cout << "GPU: " << properties.deviceName << '\n';
	std::cout << "Type: " << properties.deviceType << '\n';
	// TODO: Add mem
	// std::cout << "VRAM: " << properties. << '\n';
	std::cout << "Max texture size: "
		<< properties.limits.maxImageDimension2D
		<< "x"
		<< properties.limits.maxImageDimension2D
		<< '\n';
	std::cout << "Vendor: 0x" << std::hex << properties.vendorID << std::dec << '\n';
	std::cout << "Vulkan Version: " 
		<< VK_API_VERSION_MAJOR(properties.apiVersion) << '.'
		<< VK_API_VERSION_MINOR(properties.apiVersion) << '.'
		<< VK_API_VERSION_PATCH(properties.apiVersion)
		<< '\n';
}

void logLogicalDevice(VulkanContext &ctx)
{

	std::cout << "-------------------------------------" << '\n';
	std::cout << "Logical device information: " << '\n';
	std::cout << "Queues:\n";
	std::cout << "\tGraphics family: "
		<< ctx.queueContext.graphicsQueueFamily << '\n';
	std::cout << "\tPresent family: "
		<< ctx.queueContext.presentQueueFamily << '\n';
	std::cout << "Enabled device extensions:\n";
	for (const auto& ext : VConfig::deviceExtensions)
	{
		std::cout << "\t" << ext << '\n';
	}


}