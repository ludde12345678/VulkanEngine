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

    // create Image Views
    std::vector<VkImageView> imageViews;
    imageViews.reserve(imageCount);
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
        imageViews.push_back(view);
    }

     // store in context
    SwapchainContext context{};
    context.swapchain = swapchain;
    context.status = SwapchainStatus::Ok;
    context.images = images;
    context.imageViews = imageViews;
    context.imageFormat = swapCreateInfo.imageFormat;
    context.extent = swapChainSupport.capabilities.currentExtent;
    context.presentMode = swapCreateInfo.presentMode;
    context.imageLayouts.assign(
        imageCount,
        VK_IMAGE_LAYOUT_UNDEFINED
    );
    
    ctx.swapchainContext = context;

    if (VConfig::PRINT_DEBUG_INFO) {
        logSwapChainSupportDetails(swapChainSupport, context);
    }

}
void recreateSwapchain(VulkanContext &ctx, GLFWwindow* window) {

}

void destroySwapchain(VulkanContext &ctx) {
    for (auto imageView : ctx.swapchainContext.imageViews) {
        vkDestroyImageView(ctx.device, imageView, nullptr);
    }
        
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
VkPresentModeKHR selectPresentMode(SwapChainSupportDetails& swapChainSupport) {
    
    for (auto mode : swapChainSupport.presentModes) {
        if (mode == VConfig::preferredPresentMode) {
            return mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D selectSwapExtent(SwapChainSupportDetails& swapChainSupport, GLFWwindow* window) {
    return swapChainSupport.capabilities.currentExtent;
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
    std::cout << "\n\Selected Format:\n";
    std::cout << formatToString(context.imageFormat) << "\n";

    std::cout << "\Selected Presentation Mode:\n";
    std::cout << presentModeToString(context.presentMode) << "\n";
}


