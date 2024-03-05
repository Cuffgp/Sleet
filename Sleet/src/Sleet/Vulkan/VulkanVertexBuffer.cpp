#include "slpch.h"

#include "Sleet/Vulkan/VulkanVertexBuffer.h"

namespace Sleet {

	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size, VertexInput input) :
		m_Size(size), m_Input(input)
	{
		m_Buffer = CreateScope<VulkanBuffer>(
			m_Size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		SetData(data);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{

	}

	void VulkanVertexBuffer::SetData(void* data)
	{
		auto device = VulkanDevice::Get().Device();

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
	void VulkanVertexBuffer::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer vertexBuffers[] = { m_Buffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}
	*/

}