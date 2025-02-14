#include "slpch.h"

#include "Sleet/Renderer/Texture.h"
#include "Sleet/Vulkan/VulkanTexture.h"

namespace Sleet {

	Ref<Texture> Texture::Create(const std::string filepath)
	{
		return CreateRef<VulkanTexture>(filepath);
	}

	Ref<Texture> Texture::Create(uint32_t width, uint32_t height, void* data)
	{
		return CreateRef<VulkanTexture>(width, height, data);
	}

}