#include "slpch.h"

#include "Sleet/Vulkan/VulkanDescriptorSet.h"
#include "Sleet/Vulkan/VulkanBuffer.h"
#include "Sleet/Vulkan/VulkanTexture.h"

#include "stb_image.h"

namespace Sleet {

	VulkanTexture::VulkanTexture(const std::string filepath)
	{
		auto device = VulkanDevice::Get().Device();

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		uint32_t imageSize = texWidth * texHeight * 4;

		m_Width = texWidth;
		m_Height = texHeight;

		if (!pixels) 
		{
			throw std::runtime_error("failed to load texture image!");
		}

		CreateImage(imageSize, pixels);
		CreateSampler();

		stbi_image_free(pixels);

	}

	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, void* data)
	{
		CreateImage(width * height * 4, data);
	}

	VulkanTexture::~VulkanTexture()
	{
		auto device = VulkanDevice::Get().Device();

		vkDestroyImage(device, m_Image, nullptr);
		vkFreeMemory(device, m_ImageMemory, nullptr);
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkDestroySampler(device, m_Sampler, nullptr);
	}

	void VulkanTexture::CreateImage(uint32_t size, void* data)
	{
		auto device = VulkanDevice::Get().Device();

		VulkanBuffer stagingBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(data);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(m_Width);
		imageInfo.extent.height = static_cast<uint32_t>(m_Height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		VulkanDevice::Get().CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

		VkCommandBuffer commandBuffer = VulkanDevice::Get().BeginSingleTimeCommands();

		VulkanDevice::Get().SetImageLayout(
			commandBuffer,
			m_Image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VulkanDevice::Get().CopyBufferToImage(commandBuffer, stagingBuffer.GetBuffer(), m_Image, m_Width, m_Height);

		VulkanDevice::Get().SetImageLayout(
			commandBuffer,
			m_Image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VulkanDevice::Get().EndSingleTimeCommands(commandBuffer);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(device, &viewInfo, nullptr, &m_ImageView);
	}

	void VulkanTexture::CreateSampler()
	{
		auto device = VulkanDevice::Get().Device();

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = VulkanDevice::Get().PhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler);
	}

	void VulkanTexture::Write(Ref<DescriptorSet> descriptorSet, uint32_t binding)
	{
		auto device = VulkanDevice::Get().Device();

		auto set = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet)->GetDescriptorSet();

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_ImageView;
		imageInfo.sampler = m_Sampler;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = set;
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}


}