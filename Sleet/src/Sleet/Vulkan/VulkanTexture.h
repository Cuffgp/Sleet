#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanBuffer.h"

namespace Sleet{

	class VulkanTexture
	{
	public:

		VulkanTexture(VulkanDevice &device, const std::string& filepath);
		~VulkanTexture();

		VulkanTexture(const VulkanTexture&) = delete;
		VulkanTexture& operator=(const VulkanTexture&) = delete;

		VkDescriptorImageInfo descriptorInfo();

	private:

		VulkanDevice& device;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};

}