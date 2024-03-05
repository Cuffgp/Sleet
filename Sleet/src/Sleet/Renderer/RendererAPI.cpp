#include "slpch.h"

#include "Sleet/Renderer/RendererAPI.h"

#include "Sleet/Vulkan/VulkanRenderer.h"

namespace Sleet {

	RendererAPI* RendererAPI::Create()
	{
		return new VulkanRenderer();
	}


}