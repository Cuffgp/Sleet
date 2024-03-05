#pragma once

#include "Sleet/Renderer/IndexBuffer.h"

#include "Sleet/Vulkan/VulkanBuffer.h"

namespace Sleet {

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(void* data, uint32_t size);
		~VulkanIndexBuffer();

		virtual uint32_t GetIndexCount() override { return m_IndexCount; }
		VkBuffer GetBuffer() { return m_Buffer->GetBuffer(); }
	private:
		void SetData(void* data);

		Scope<VulkanBuffer> m_Buffer;
		uint32_t m_Size;
		uint32_t m_IndexCount;
	};

}