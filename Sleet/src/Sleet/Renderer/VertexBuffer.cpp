#include "slpch.h"

#include "Sleet/Renderer/RendererAPI.h"
#include "Sleet/Renderer/VertexBuffer.h"

#include "Sleet/Vulkan/VulkanVertexBuffer.h"

namespace Sleet {

	Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size)
	{
		return CreateRef<VulkanVertexBuffer>(data, size); 
	}


}