#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"

namespace Sleet {

	class VulkanBuffer
	{
	public:
		VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		~VulkanBuffer();

		void Map();
		void Unmap();
		void WriteToBuffer(void* data);
		VkBuffer GetBuffer() { return m_Buffer; }
	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_Memory;
		VkDeviceSize m_Size;
		void* m_Mapped = nullptr;
	};

}