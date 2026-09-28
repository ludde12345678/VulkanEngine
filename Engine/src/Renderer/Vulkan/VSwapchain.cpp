#include "VSwapchain.h"

void createSwapChain(VulkanContext& ctx, GLFWwindow* window) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(ctx);

    uint32_t imageCount = std::max(VConfig::preferredSwapchainImages, swapChainSupport.capabilities.minImageCount);
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR swapCreateInfo{};
    swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreateInfo.surface = ctx.surface;
    swapCreateInfo.minImageCount = imageCount; // triple buffering
    VkSurfaceFormatKHR surfaceFormat = selectSwapSurfaceFormat(swapChainSupport);
    swapCreateInfo.imageFormat = surfaceFormat.format;
    swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

    swapCreateInfo.imageExtent = selectSwapExtent(swapChainSupport, window);

    swapCreateInfo.imageArrayLayers = 1;
    swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {
    ctx.queueContext.graphicsQueueFamily,
    ctx.queueContext.presentQueueFamily
    };
    if (ctx.queueContext.graphicsQueueFamily == ctx.queueContext.presentQueueFamily) {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapCreateInfo.queueFamilyIndexCount = 0;
        swapCreateInfo.pQueueFamilyIndices = nullptr;
    }
    else
    {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapCreateInfo.queueFamilyIndexCount = 2;
        swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    swapCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // fricking importants,
    //TODO: dont hardcode this/theis whole struct
    swapCreateInfo.presentMode = selectPresentMode(swapChainSupport);
    swapCreateInfo.clipped = VK_TRUE;
    swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    if(vkCreateSwapchainKHR(ctx.device, &swapCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swapchain");
    }

    // get and store images
    vkGetSwapchainImagesKHR(ctx.device, swapchain, &imageCount, nullptr);
    std::vector<VkImage>images(imageCount);
    vkGetSwapchainImagesKHR(ctx.device, swapchain, &imageCount, images.data());

    // create Image Views/render targets
    std::vector<RenderTarget> colorTargets;
    colorTargets.reserve(imageCount);
    // depth
    VkFormat depthFormat = selectDepthFormat(ctx);
    std::vector<AllocatedImage>depthImages;
    std::vector<RenderTarget> depthTargets;
    depthImages.reserve(imageCount);
    depthTargets.reserve(imageCount);

    for (VkImage image : images) {
        VkImageViewCreateInfo ViewCreateInfo{};
        ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewCreateInfo.image = image;
        ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ViewCreateInfo.format = surfaceFormat.format;

        ViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ViewCreateInfo.subresourceRange.levelCount = 1;
        ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ViewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView view;
        if(vkCreateImageView(ctx.device, &ViewCreateInfo, nullptr, &view) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }
        colorTargets.push_back(RenderTarget{
            .image = image,
            .imageView = view,
            .format = surfaceFormat.format,
            .extent = swapCreateInfo.imageExtent,
            .layout = VK_IMAGE_LAYOUT_UNDEFINED
            }
        
        );

        // depth images:
        VkImageCreateInfo depthImageCreate{};
        depthImageCreate.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageCreate.imageType = VK_IMAGE_TYPE_2D;
        depthImageCreate.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        depthImageCreate.extent = {
            swapCreateInfo.imageExtent.width,
            swapCreateInfo.imageExtent.height,
            1
        };
        depthImageCreate.mipLevels = 1;
        depthImageCreate.arrayLayers = 1;
        depthImageCreate.flags = 0;

        depthImageCreate.format = depthFormat;
        depthImageCreate.tiling = VK_IMAGE_TILING_OPTIMAL;

        depthImageCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        depthImageCreate.samples = VK_SAMPLE_COUNT_1_BIT;

        depthImageCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkImage depthImage;
        VK_CHECK(vkCreateImage(ctx.device, &depthImageCreate, nullptr, &depthImage), "Error creating depth images");

        // allocate
        AllocatedImage allocImage = allocateImage(ctx, depthImage);

        VkImageViewCreateInfo ViewCreateDepthInfo{};
        ViewCreateDepthInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewCreateDepthInfo.image = depthImage;
        ViewCreateDepthInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ViewCreateDepthInfo.format = depthFormat;

        ViewCreateDepthInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        ViewCreateDepthInfo.subresourceRange.baseMipLevel = 0;
        ViewCreateDepthInfo.subresourceRange.levelCount = 1;
        ViewCreateDepthInfo.subresourceRange.baseArrayLayer = 0;
        ViewCreateDepthInfo.subresourceRange.layerCount = 1;

        VkImageView depthview;
        if (vkCreateImageView(ctx.device, &ViewCreateDepthInfo, nullptr, &depthview) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }

        // assing other values
        allocImage.view = depthview;
        allocImage.format = depthFormat;
        allocImage.extent = {
            swapCreateInfo.imageExtent.width,
            swapCreateInfo.imageExtent.height,
        };
        depthImages.push_back(allocImage);
        RenderTarget depthTarget{
        .image = depthImage,
        .imageView = depthview,
        .format = depthFormat,
        .extent = swapCreateInfo.imageExtent,
        .layout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        
        depthTargets.push_back(depthTarget);

    }

     // store in context
    SwapchainContext context{};
    context.swapchain = swapchain;
    context.status = SwapchainStatus::Ok;
    context.imageCount = imageCount;

    context.presentMode = swapCreateInfo.presentMode;

    
    context.allocatedDepthImages = depthImages;
    context.colorTargets = colorTargets;
    context.depthTargets = depthTargets;
    ctx.swapchainContext = context;

    if (VConfig::PRINT_DEBUG_INFO) {
        logSwapChainSupportDetails(swapChainSupport, context);
    }

}
void recreateSwapchain(VulkanContext &ctx, GLFWwindow* window) {

    destroySwapchain(ctx);

    createSwapChain(ctx, window);
}

void destroySwapchain(VulkanContext &ctx) {

    for (auto Allocimage : ctx.swapchainContext.allocatedDepthImages) {
        destroyImage(ctx, Allocimage);
    }
    ctx.swapchainContext.depthTargets.clear();

    for (auto target : ctx.swapchainContext.colorTargets) {
        vkDestroyImageView(ctx.device, target.imageView, nullptr);
    }
    ctx.swapchainContext.colorTargets.clear();
    vkDestroySwapchainKHR(ctx.device, ctx.swapchainContext.swapchain, nullptr);
}

VkSurfaceFormatKHR selectSwapSurfaceFormat(SwapChainSupportDetails& swapChainSupport) {
    
    for (auto format : swapChainSupport.formats) {
        if(format.format == VConfig::preferredSwapFormat) {
            return format;
        }
    }
    return swapChainSupport.formats[0];
}
VkFormat selectDepthFormat(VulkanContext& ctx) {

    for (auto& format : VConfig::preferredDepthFormats) {
        VkFormatProperties props{};
        vkGetPhysicalDeviceFormatProperties(
            ctx.physicalDevice,
            format,
            &props
        );
        if (props.optimalTilingFeatures &
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            return format;
        }
    }
    throw std::runtime_error("ERROR: No suitable depth format found");
    
}
VkPresentModeKHR selectPresentMode(SwapChainSupportDetails& swapChainSupport) {
    
    for (auto mode : swapChainSupport.presentModes) {
        if (mode == VConfig::preferredPresentMode) {
            return mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D selectSwapExtent(SwapChainSupportDetails& swapChainSupport, GLFWwindow* window) {
    if (swapChainSupport.capabilities.currentExtent.width != UINT32_MAX)
    {
        return swapChainSupport.capabilities.currentExtent;
    }
    int width;
    int height;

    glfwGetFramebufferSize(
        window,
        &width,
        &height
    );

    VkExtent2D actualExtent =
    {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width =
        std::clamp(
            actualExtent.width,
            swapChainSupport.capabilities.minImageExtent.width,
            swapChainSupport.capabilities.maxImageExtent.width
        );

    actualExtent.height =
        std::clamp(
            actualExtent.height,
            swapChainSupport.capabilities.minImageExtent.height,
            swapChainSupport.capabilities.maxImageExtent.height
        );

    return actualExtent;
}


SwapChainSupportDetails querySwapChainSupport(VulkanContext& ctx) {
    SwapChainSupportDetails details;

    // surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.physicalDevice,ctx.surface, &details.capabilities );
    // formats
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.physicalDevice, ctx.surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.physicalDevice, ctx.surface, &formatCount, details.formats.data());
    }
    // Presentation Modes
    uint32_t presentCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.physicalDevice, ctx.surface, &presentCount, nullptr);

    if (presentCount != 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.physicalDevice, ctx.surface, &presentCount, details.presentModes.data());
    }
    return details;
}
// SwapChainSupport logging function
void logSwapChainSupportDetails(const SwapChainSupportDetails& details, const SwapchainContext& context) {
    std::cout << "\n----------------------";
    std::cout << "\nSurface Capabilities:\n";
    std::cout << "Min image count:" << details.capabilities.minImageCount << "\n";
    std::cout << "Max image count:" << details.capabilities.maxImageCount << "\n";
    std::cout << "Current Extent:" << details.capabilities.currentExtent.width << "x" << details.capabilities.currentExtent.height << "\n";
    std::cout << "Current Transform:" << details.capabilities.currentTransform << "\n";

    // log formats
    std::cout << "\nSupported Formats:\n";
    for (auto format : details.formats) {
        std::cout << formatToString(format.format) << "\n";
        std::cout << colorSpaceToString(format.colorSpace) << "\n";
    }
    // log present
    std::cout << "\nSupported Presentation Modes:\n";
    for (auto present : details.presentModes) {
        std::cout << presentModeToString(present) << "\n";
    }
    std::cout << "\nSelected Format:\n";
    std::cout << formatToString(context.colorTargets[0].format) << "\n";

    std::cout << "\nSelected Depth Format:\n";
    std::cout << formatToString(context.depthTargets[0].format) << "\n";


    std::cout << "\Selected Presentation Mode:\n";
    std::cout << presentModeToString(context.presentMode) << "\n";
}


