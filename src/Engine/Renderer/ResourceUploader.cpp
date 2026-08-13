#include "ResourceUploader.h"


void populateMesh(RendererContext& ctx, Mesh* mesh, std::vector<Vertex> vert, std::vector<uint32_t> indices) {
	if (vert.size() > 0) {
		mesh->vertexBuffer = createVertexBuffer<Vertex>(ctx.vulkanContext, vert);
	}
	mesh->vertexCount = vert.size();
	if (indices.size() > 0) {
		mesh->indexBuffer = createIndexBuffer<uint32_t>(ctx.vulkanContext, indices);
	}
	mesh->indexCount = indices.size();
}

void populateTexture(RendererContext& ctx, Texture* texture, TextureData& texData) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = texData.width;
	imageInfo.extent.height = texData.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VkImage image;
	VK_CHECK(vkCreateImage(ctx.vulkanContext.device, &imageInfo, nullptr, &image), "Failed to create image");

	AllocatedImage allocImage = createTextureImage(ctx.vulkanContext, image, texData.pixels, { texData.width, texData.height });	

	// create image view
        VkImageViewCreateInfo ViewCreateInfo{};
        ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewCreateInfo.image = allocImage.image;
        ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;

        ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ViewCreateInfo.subresourceRange.levelCount = 1;
        ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ViewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
	VK_CHECK(vkCreateImageView(ctx.vulkanContext.device, &ViewCreateInfo, nullptr, &imageView), "Failed to create image view");
	allocImage.view = imageView;

	// create sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	VK_CHECK(vkCreateSampler(ctx.vulkanContext.device, &samplerInfo, nullptr, &texture->sampler), "Failed to create sampler");


	texture->image = allocImage;
}