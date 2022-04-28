#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanBuffer.h"

namespace Sleet{

	class VulkanTexture
	{
	public:

		VulkanTexture(VulkanDevice& device, void* data, uint32_t texWidth, uint32_t texHeight);
		~VulkanTexture();

		VulkanTexture(const VulkanTexture&) = delete;
		VulkanTexture& operator=(const VulkanTexture&) = delete;

		void createTexture(void* data, uint32_t width, uint32_t height);
		VkDescriptorImageInfo descriptorInfo();

		static Scope<VulkanTexture> createTextureFromFile(VulkanDevice& device, const std::string& filepath);
		static Scope<VulkanTexture> createTexture(VulkanDevice& device, void* data, uint32_t width, uint32_t height);

	private:

		VulkanDevice& device;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};

}