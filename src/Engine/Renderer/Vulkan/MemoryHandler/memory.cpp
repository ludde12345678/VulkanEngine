#include "memory.h"
#include "../VCommands.h"


// private
namespace MemoryInternal {
	void copyBuffer(VulkanContext& ctx, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBuffer cmd = beginSingleTimeCommands(ctx);
		VkBufferCopy2 copyRegion{};
		copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		VkCopyBufferInfo2 bufCopyInfo{};
		bufCopyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
		bufCopyInfo.srcBuffer = srcBuffer;
		bufCopyInfo.dstBuffer = dstBuffer;
		bufCopyInfo.regionCount = 1;
		bufCopyInfo.pRegions = &copyRegion;

		vkCmdCopyBuffer2(cmd, &bufCopyInfo);

		endSingleTimeCommands(ctx, cmd);

	}

	AllocatedBuffer createBuffer(VulkanContext& ctx, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {

		VkBuffer buf;
		VkBufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.usage = usage;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.size = size;

		VK_CHECK(vkCreateBuffer(ctx.device, &info, nullptr, &buf), "Create buffer failed!");

		VkBufferMemoryRequirementsInfo2 reqInfo{};
		reqInfo.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		reqInfo.buffer = buf;

		VkMemoryRequirements2 requirements{};
		requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

		vkGetBufferMemoryRequirements2(ctx.device, &reqInfo, &requirements);

		VkDeviceMemory mem;
		allocateMemory(requirements, ctx, properties, mem);

		VkBindBufferMemoryInfo bindInfo{};
		bindInfo.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
		bindInfo.buffer = buf;
		bindInfo.memory = mem;
		bindInfo.memoryOffset = 0;

		VK_CHECK(vkBindBufferMemory2(ctx.device, 1, &bindInfo), "Bind Buffer Memory Failed");
		AllocatedBuffer AllocBuf{
			.buffer = buf,
			.memory = mem,
			.size = size
		};

		return AllocBuf;
	}
	void allocateMemory(VkMemoryRequirements2& requirements, VulkanContext& ctx, VkMemoryPropertyFlags properties, VkDeviceMemory& mem)
	{
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = requirements.memoryRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(
			ctx.physicalDevice,
			requirements.memoryRequirements.memoryTypeBits,
			properties
		);
		vkAllocateMemory(ctx.device, &allocInfo, nullptr, &mem);
	}
	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(
			physicalDevice,
			&memProperties
		);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

}

AllocatedBuffer createUniformBuffer(VulkanContext& ctx, VkDeviceSize size) {
	return MemoryInternal::createBuffer(ctx, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
AllocatedImage createImage(VulkanContext& ctx, VkImage image) {
	
	VkImageMemoryRequirementsInfo2 reqInfo{};
	reqInfo.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
	reqInfo.image = image;
	
	VkMemoryRequirements2 requirements{};
	requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

	vkGetImageMemoryRequirements2(
		ctx.device, 
		&reqInfo,
		&requirements);
	VkDeviceMemory mem;
	MemoryInternal::allocateMemory(requirements, ctx, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mem);


	VkBindImageMemoryInfo bindInfo{};
	bindInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
	bindInfo.image = image;
	bindInfo.memory = mem;
	bindInfo.memoryOffset = 0;

	VK_CHECK(vkBindImageMemory2(ctx.device, 1, &bindInfo), "Bind Image Memory Failed");
	AllocatedImage AllocImg{};
	AllocImg.image = image;
	AllocImg.memory = mem;
	AllocImg.size = requirements.memoryRequirements.size;
	return AllocImg;


}
void destroyImage(VulkanContext& ctx, AllocatedImage& img) {
	if (img.image != VK_NULL_HANDLE)
	{
		vkDestroyImage(ctx.device, img.image, nullptr);
		img.image = VK_NULL_HANDLE;
	}

	if (img.memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(ctx.device, img.memory, nullptr);
		img.memory = VK_NULL_HANDLE;
	}
}

void destroyBuffer(VulkanContext& ctx, AllocatedBuffer& buf) {
	if (buf.buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(ctx.device, buf.buffer, nullptr);
		buf.buffer = VK_NULL_HANDLE;
	}

	if (buf.memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(ctx.device, buf.memory, nullptr);
		buf.memory = VK_NULL_HANDLE;
	}
}




