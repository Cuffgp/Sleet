#include "slpch.h"

#include "Sleet/Renderer/Texture.h"
#include "Sleet/Vulkan/VulkanTexture.h"

namespace Sleet {

	Ref<Texture> Texture::Create(const std::string filepath)
	{
		return CreateRef<VulkanTexture>(filepath);
	}

}