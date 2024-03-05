#include "slpch.h"

#include "Sleet/Vulkan/VulkanIndexBuffer.h"

namespace Sleet {

	VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t size) :
		m_Size(size)
	{
		m_IndexCount = size / sizeof(uint32_t);

		m_Buffer = CreateScope<VulkanBuffer>(
			m_Size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		SetData(data);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{

	}

	void VulkanIndexBuffer::SetData(void* data)
	{

		VulkanBuffer stagingBuffer = VulkanBuffer(
			m_Size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(data);
		VulkanDevice::Get().CopyBuffer(stagingBuffer.GetBuffer(), m_Buffer->GetBuffer(), m_Size);
	}

	/*
	void VulkanIndexBuffer::Bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, m_Buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
	*/
}