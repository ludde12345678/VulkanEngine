#include "ResourceUploader.h"


void createDefaultTexture(RendererContext& ctx, std::vector<Texture>& textures) {
    // Albedo: white
    TextureData albedoData{};
    albedoData.height = 100;
    albedoData.width = 100;
    albedoData.name = "Default white albedo texture";
    albedoData.pixels.resize(albedoData.width * albedoData.height * 4);

    for (size_t i = 0; i < albedoData.pixels.size(); i += 4)
    {
        albedoData.pixels[i + 0] = 255;
        albedoData.pixels[i + 1] = 255;
        albedoData.pixels[i + 2] = 255;
        albedoData.pixels[i + 3] = 255;
    }

    Texture albedo;
    populateTexture(ctx, &albedo, albedoData);
    textures.push_back(albedo);


    // Normal: flat normal (0, 0, 1)
    TextureData normalData{};
    normalData.height = 100;
    normalData.width = 100;
    normalData.name = "Default flat normal texture";
    normalData.pixels.resize(normalData.width * normalData.height * 4);

    for (size_t i = 0; i < normalData.pixels.size(); i += 4)
    {
        normalData.pixels[i + 0] = 128;
        normalData.pixels[i + 1] = 128;
        normalData.pixels[i + 2] = 255;
        normalData.pixels[i + 3] = 255;
    }

    Texture normal;
    populateTexture(ctx, &normal, normalData);
    textures.push_back(normal);


    // Roughness: fully rough
    TextureData roughnessData{};
    roughnessData.height = 100;
    roughnessData.width = 100;
    roughnessData.name = "Default roughness texture";
    roughnessData.pixels.resize(roughnessData.width * roughnessData.height * 4);

    for (size_t i = 0; i < roughnessData.pixels.size(); i += 4)
    {
        roughnessData.pixels[i + 0] = 255;
        roughnessData.pixels[i + 1] = 255;
        roughnessData.pixels[i + 2] = 255;
        roughnessData.pixels[i + 3] = 255;
    }

    Texture roughness;
    populateTexture(ctx, &roughness, roughnessData);
    textures.push_back(roughness);


    // Metallic: non-metal
    TextureData metallicData{};
    metallicData.height = 100;
    metallicData.width = 100;
    metallicData.name = "Default non-metallic texture";
    metallicData.pixels.resize(metallicData.width * metallicData.height * 4);

    for (size_t i = 0; i < metallicData.pixels.size(); i += 4)
    {
        metallicData.pixels[i + 0] = 0;
        metallicData.pixels[i + 1] = 0;
        metallicData.pixels[i + 2] = 0;
        metallicData.pixels[i + 3] = 255;
    }

    Texture metallic;
    populateTexture(ctx, &metallic, metallicData);
    textures.push_back(metallic);


    // AO: no occlusion
    TextureData aoData{};
    aoData.height = 100;
    aoData.width = 100;
    aoData.name = "Default ambient occlusion texture";
    aoData.pixels.resize(aoData.width * aoData.height * 4);

    for (size_t i = 0; i < aoData.pixels.size(); i += 4)
    {
        aoData.pixels[i + 0] = 255;
        aoData.pixels[i + 1] = 255;
        aoData.pixels[i + 2] = 255;
        aoData.pixels[i + 3] = 255;
    }

    Texture ao;
    populateTexture(ctx, &ao, aoData);
    textures.push_back(ao);

}

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