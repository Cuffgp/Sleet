#include "slpch.h"

#include "Sleet/Renderer/Framebuffer.h"

#include "Sleet/Vulkan/VulkanFramebuffer.h"

namespace Sleet {

	Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height)
	{
		return CreateRef<VulkanFramebuffer>(width, height);
	}

	Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height, std::vector<ImageFormat> attachments)
	{
		return CreateRef<VulkanFramebuffer>(width, height, attachments);
	}
}