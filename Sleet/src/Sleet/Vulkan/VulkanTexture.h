#pragma once

#include "Sleet/Renderer/Texture.h"

#include "Sleet/Vulkan/VulkanDevice.h"

namespace Sleet {

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string filePath);
		~VulkanTexture();

		virtual void Write(Ref<DescriptorSet> descriptorSet, uint32_t binding) override;

	private:
		VkImage m_Image;
		VkDeviceMemory m_ImageMemory;
		VkImageView m_ImageView;
		VkSampler m_Sampler;

		uint32_t m_Width;
		uint32_t m_Height;
	};

}