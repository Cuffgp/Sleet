#include "slpch.h"

#include "Sleet/Renderer/RendererAPI.h"
#include "Sleet/Renderer/UniformBuffer.h"

#include "Sleet/Vulkan/VulkanUniformBuffer.h"

namespace Sleet {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		return CreateRef<VulkanUniformBuffer>(size);
	}
}