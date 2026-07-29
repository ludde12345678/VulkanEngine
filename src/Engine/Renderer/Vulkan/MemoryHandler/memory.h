#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <volk.h>
#include <iostream>

#include "MemStructs.h"
#include "../VContext.h"
#include "../VUtils.h"

namespace MemoryInternal
{
	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	AllocatedBuffer createBuffer(VulkanContext& ctx, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void copyBuffer(VulkanContext& ctx, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	template<typename T>
	AllocatedBuffer createStagingBuffer(VulkanContext& ctx, const std::vector<T>& data, VkDeviceSize size) {

	AllocatedBuffer buf = createBuffer(ctx, size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* mapped;

		vkMapMemory(ctx.device, buf.memory, 0, size, 0, &mapped);

		memcpy(mapped, data.data(), size);

		vkUnmapMemory(ctx.device, buf.memory);
		return buf;
	}
}

void destroyBuffer(VulkanContext& ctx, AllocatedBuffer& buf);

AllocatedBuffer createUniformBuffer(VulkanContext& ctx, VkDeviceSize size);

template<typename T>
AllocatedBuffer createVertexBuffer(VulkanContext& ctx, const std::vector<T>& data) {
	VkDeviceSize size = sizeof(T) * data.size();
	AllocatedBuffer StagingBuffer = MemoryInternal::createStagingBuffer<T>(ctx, data, size);
	AllocatedBuffer buffer = MemoryInternal::createBuffer(ctx, size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	MemoryInternal::copyBuffer(ctx, StagingBuffer.buffer, buffer.buffer, size);
	destroyBuffer(ctx, StagingBuffer);
	return buffer;


}
template<typename T>
AllocatedBuffer createIndexBuffer(VulkanContext& ctx, const std::vector<T>& data) {
	VkDeviceSize size = sizeof(T) * data.size();
	AllocatedBuffer StagingBuffer = MemoryInternal::createStagingBuffer<T>(ctx, data, size);
	AllocatedBuffer buffer = MemoryInternal::createBuffer(ctx, size,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	MemoryInternal::copyBuffer(ctx, StagingBuffer.buffer, buffer.buffer, size);
	destroyBuffer(ctx, StagingBuffer);
	return buffer;
}


template<typename T>
void updateUniformBuffer(VulkanContext& ctx, AllocatedBuffer& buffer, const T& data)
{
	void* mapped;
	vkMapMemory(ctx.device, buffer.memory, 0,sizeof(T), 0, &mapped);
	memcpy(mapped, &data, sizeof(T));
	vkUnmapMemory(ctx.device, buffer.memory);
}