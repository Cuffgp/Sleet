#include "slpch.h"

#include "Sleet/Renderer/IndexBuffer.h"
#include "Sleet/Renderer/RendererAPI.h"

#include "Sleet/Vulkan/VulkanIndexBuffer.h"

namespace Sleet {

	Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t size)
	{
		return CreateRef<VulkanIndexBuffer>(data, size); 
	}


}