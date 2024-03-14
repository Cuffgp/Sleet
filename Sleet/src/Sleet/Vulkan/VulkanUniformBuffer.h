#pragma once

#include "Sleet/Renderer/Pipeline.h"
#include "Sleet/Renderer/UniformBuffer.h"

#include "Sleet/Vulkan/VulkanBuffer.h"

namespace Sleet {

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size);
		~VulkanUniformBuffer();

		virtual void SetData(void* data) override;

		virtual void Write(Ref<DescriptorSet> descriptorSet, uint32_t binding) override;

		void WriteToSet(VkDescriptorSet set, uint32_t binding);
	private:
		uint32_t m_Size;
		Scope<VulkanBuffer> m_Buffer;
		VkDescriptorBufferInfo m_DescriptorInfo;
	};

}