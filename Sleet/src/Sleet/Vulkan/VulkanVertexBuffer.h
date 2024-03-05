#pragma once

#include "Sleet/Renderer/ShaderData.h"
#include "Sleet/Renderer/VertexBuffer.h"

#include "Sleet/Vulkan/VulkanBuffer.h"

namespace Sleet {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(void* data, uint32_t size, VertexInput input);
		~VulkanVertexBuffer();

		virtual const VertexInput& GetInput() const override { return m_Input; }
		virtual void SetData(void* data) override;

		VkBuffer GetBuffer() { return m_Buffer->GetBuffer(); }
	private:
		uint32_t m_Size;
		VertexInput m_Input;
		Scope<VulkanBuffer> m_Buffer;
	};

}