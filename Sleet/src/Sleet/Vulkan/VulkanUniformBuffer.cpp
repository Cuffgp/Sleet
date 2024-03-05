#include "slpch.h"

#include "Sleet/Vulkan/VulkanDescriptorSet.h"
#include "Sleet/Vulkan/VulkanUniformBuffer.h"

namespace Sleet {

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size):
		m_Size(size)
	{
		m_Buffer = CreateScope<VulkanBuffer>(
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		m_DescriptorInfo.buffer = m_Buffer->GetBuffer();
		m_DescriptorInfo.offset = 0;
		m_DescriptorInfo.range = m_Size;
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}

	void VulkanUniformBuffer::SetData(void* data)
	{
		m_Buffer->Map();
		m_Buffer->WriteToBuffer(data);
	}

	void VulkanUniformBuffer::Write(Ref<DescriptorSet> descriptorSet)
	{
		auto set = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet)->GetDescriptorSet();
		WriteToSet(set, 0);
	}

	void VulkanUniformBuffer::WriteToSet(VkDescriptorSet set, uint32_t binding)
	{
		auto device = VulkanDevice::Get().Device();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = set;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &m_DescriptorInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}

}