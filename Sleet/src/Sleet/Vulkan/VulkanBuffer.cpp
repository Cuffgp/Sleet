#include "slpch.h"

#include "Sleet/Vulkan/VulkanBuffer.h"

namespace Sleet {

	VulkanBuffer::VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties):
		m_Size(size)
	{
		auto device = VulkanDevice::Get().Device();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
		{
			SL_ERROR("Failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanDevice::Get().FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
		{
			SL_ERROR("Failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, m_Buffer, m_Memory, 0);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		auto device = VulkanDevice::Get().Device();

		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_Memory, nullptr);
	}

	void VulkanBuffer::Map()
	{
		auto device = VulkanDevice::Get().Device();

		vkMapMemory(device, m_Memory, 0, m_Size, 0, &m_Mapped);
	}

	void VulkanBuffer::Unmap()
	{
		auto device = VulkanDevice::Get().Device();

		vkUnmapMemory(device, m_Memory);
		m_Mapped = nullptr;
	}

	void VulkanBuffer::WriteToBuffer(void* data)
	{
		SL_ASSERT(m_Mapped, "Memory must be mapped to write");
		memcpy(m_Mapped, data, m_Size);
	}

}