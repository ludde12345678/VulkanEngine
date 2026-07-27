#pragma once


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <volk.h>
#include <iostream>
#include "VContext.h"
#include "VConfig.h"
#include "VUtils.h"


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

void createSwapChain(VulkanContext& ctx, GLFWwindow* window);

void recreateSwapchain(VulkanContext& ctx, GLFWwindow* window);

void destroySwapchain(VulkanContext& ctx);

VkSurfaceFormatKHR selectSwapSurfaceFormat(SwapChainSupportDetails& swapChainSupport);

VkPresentModeKHR selectPresentMode(SwapChainSupportDetails& swapChainSupport);

VkExtent2D selectSwapExtent(SwapChainSupportDetails& swapChainSupport, GLFWwindow* window);

SwapChainSupportDetails querySwapChainSupport(VulkanContext& ctx);

void logSwapChainSupportDetails(const SwapChainSupportDetails& details, const SwapchainContext& context);


// Debug helper functions
inline const char* formatToString(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_B8G8R8A8_SRGB:
        return "B8G8R8A8_SRGB";

    case VK_FORMAT_R8G8B8A8_SRGB:
        return "R8G8B8A8_SRGB";

    case VK_FORMAT_B8G8R8A8_UNORM:
        return "B8G8R8A8_UNORM";

    default:
        return "Unknown format";
    }
}

inline const char* colorSpaceToString(VkColorSpaceKHR colorSpace)
{
    switch (colorSpace)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return "SRGB nonlinear";

    default:
        return "Unknown color space";
    }
}

inline const char* presentModeToString(VkPresentModeKHR mode)
{
    switch (mode)
    {
    case VK_PRESENT_MODE_FIFO_KHR:
        return "FIFO (VSync)";

    case VK_PRESENT_MODE_MAILBOX_KHR:
        return "MAILBOX (Triple buffering)";

    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return "IMMEDIATE (No VSync)";

    default:
        return "Unknown";
    }
}



