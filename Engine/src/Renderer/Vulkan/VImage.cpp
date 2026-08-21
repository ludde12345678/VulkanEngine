#include "VImage.h"


void transitionSwapchainImage(VulkanContext& ctx, VkCommandBuffer cmd, uint32_t imageIndex, VkImageLayout newLayout) {
	VkImageLayout oldLayout = ctx.swapchainContext.imageLayouts[imageIndex];
	transitionImageLayout(
		cmd,
		ctx.swapchainContext.images[imageIndex],
		ctx.swapchainContext.imageFormat,
		oldLayout,
		newLayout
	);
	ctx.swapchainContext.imageLayouts[imageIndex] = newLayout;

}
void transitionSwapchainDepthImage(VulkanContext& ctx, VkCommandBuffer cmd, VkImageLayout oldLayout, VkImageLayout newLayout) {
	
	for (auto& depthImage : ctx.swapchainContext.allocatedDepthImages) {
		transitionImageLayout(
			cmd,
			depthImage.image,
			depthImage.format,
			oldLayout,
			newLayout
		);
	}


	

}


void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

	VkImageSubresourceRange resourceRange{};
	resourceRange.aspectMask = getAspectFlags(format);

	resourceRange.baseMipLevel = 0;
	resourceRange.levelCount = 1;

	resourceRange.baseArrayLayer = 0;
	resourceRange.layerCount = 1;


	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = nullptr;
	barrier.image = image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.subresourceRange = resourceRange;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// if ladder from hell
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.dstAccessMask = VK_ACCESS_2_NONE;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL)
	{
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
			VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	}


	else
	{
		throw std::runtime_error("Unsupported image transition");
	}

	// check if they are set
	/*
	if (barrier.dstStageMask == 0 || barrier.srcStageMask == 0) {
		throw std::runtime_error("Transition src/dst stage mask not set!");
	}
	if (barrier.dstAccessMask == 0 || barrier.srcAccessMask == 0) {
		throw std::runtime_error("Transition src/dst access mask not set!");
	}
	*/
	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.pNext = nullptr;
	dependencyInfo.dependencyFlags = 0;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(cmd, &dependencyInfo);

}

VkImageAspectFlags getAspectFlags(VkFormat format)
{
	if (format == VK_FORMAT_D32_SFLOAT ||
		format == VK_FORMAT_D16_UNORM ||
		format == VK_FORMAT_X8_D24_UNORM_PACK32)
	{
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	if (format == VK_FORMAT_D24_UNORM_S8_UINT ||
		format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		format == VK_FORMAT_D16_UNORM_S8_UINT)
	{
		return VK_IMAGE_ASPECT_DEPTH_BIT |
			VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	return VK_IMAGE_ASPECT_COLOR_BIT;
}