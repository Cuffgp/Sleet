#include "slpch.h"

#include "Sleet/Renderer/Renderer.h"

#include "Sleet/Vulkan/VulkanSwapchain.h"
#include "Sleet/Vulkan/VulkanFramebuffer.h"
#include "Sleet/Vulkan/VulkanDescriptorSet.h"

namespace Sleet {


	VulkanFramebuffer::VulkanFramebuffer(uint32_t width, uint32_t height):
		m_Width(width), m_Height(height)
	{
		// Default to swapchain attachments

		m_Attachments.resize(Renderer::FramesInFlight());

		auto colourFormat = VulkanSwapchain::s_SwapchainImageFormat;
		auto depthFormat = VulkanSwapchain::s_SwapchainDepthFormat;

		m_DepthFormat = depthFormat;
		m_ColourFormats.push_back(colourFormat);

		for (int i = 0; i < m_Attachments.size(); i++)
		{
			m_Attachments[i].ColourAttachments.push_back(CreateColourAttachment(colourFormat));
			m_Attachments[i].DepthStencilAttachment = CreateDepthStencil(depthFormat);
		}
	}

	VulkanFramebuffer::VulkanFramebuffer(uint32_t width, uint32_t height, std::vector<ImageFormat> attachments):
		m_Width(width), m_Height(height)
	{
		for (const auto& format : attachments)
		{
			if (Utils::IsDepthStencil(format))
				m_DepthFormat = Utils::ImageFormatToVulkan(format);
			else
				m_ColourFormats.push_back(Utils::ImageFormatToVulkan(format));
		}

		for (int i = 0; i < m_Attachments.size(); i++)
		{
			m_Attachments[i].DepthStencilAttachment = CreateDepthStencil(m_DepthFormat);

			for (int j = 0; j < m_ColourFormats.size(); j++)
			{
				m_Attachments[i].ColourAttachments.push_back(CreateColourAttachment(m_ColourFormats[i]));
			}
		}

	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		auto device = VulkanDevice::Get().Device();

		for (int i = 0; i < m_Attachments.size(); i++)
		{
			for (int j = 0; j < m_Attachments[i].ColourAttachments.size(); j++)
			{
				vkDestroyImage(device, m_Attachments[i].ColourAttachments[j].Image, nullptr);
				vkFreeMemory(device, m_Attachments[i].ColourAttachments[j].ImageMemory, nullptr);
				vkDestroyImageView(device, m_Attachments[i].ColourAttachments[j].ImageView, nullptr);
				vkDestroySampler(device, m_Attachments[i].ColourAttachments[j].Sampler, nullptr);
			}

			vkDestroyImage(device, m_Attachments[i].DepthStencilAttachment.Image, nullptr);
			vkFreeMemory(device, m_Attachments[i].DepthStencilAttachment.ImageMemory, nullptr);
			vkDestroyImageView(device, m_Attachments[i].DepthStencilAttachment.ImageView, nullptr);
			vkDestroySampler(device, m_Attachments[i].DepthStencilAttachment.Sampler, nullptr);
		}
	}

	void VulkanFramebuffer::Write(Ref<DescriptorSet> descriptorSet, uint32_t binding, uint32_t frameIndex)
	{
		auto device = VulkanDevice::Get().Device();

		auto set = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet)->GetDescriptorSet();

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_Attachments[frameIndex].ColourAttachments[0].ImageView;
		imageInfo.sampler = m_Attachments[frameIndex].ColourAttachments[0].Sampler;

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

	VkPipelineRenderingCreateInfo VulkanFramebuffer::GetPipelineRenderingInfo()
	{
		VkPipelineRenderingCreateInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		renderingInfo.colorAttachmentCount = m_ColourFormats.size();
		renderingInfo.pColorAttachmentFormats = m_ColourFormats.data();
		renderingInfo.depthAttachmentFormat = m_DepthFormat;
		renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		return renderingInfo;
	}

	VulkanFramebuffer::Image VulkanFramebuffer::CreateColourAttachment(VkFormat colourFormat)
	{
		auto device = VulkanDevice::Get().Device();

		VulkanFramebuffer::Image image;

		VkImageCreateInfo colourImageInfo{};
		colourImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colourImageInfo.imageType = VK_IMAGE_TYPE_2D;
		colourImageInfo.extent.width = static_cast<uint32_t>(m_Width);
		colourImageInfo.extent.height = static_cast<uint32_t>(m_Height);
		colourImageInfo.extent.depth = 1;
		colourImageInfo.mipLevels = 1;
		colourImageInfo.arrayLayers = 1;
		colourImageInfo.format = colourFormat;
		colourImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		colourImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colourImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		colourImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		colourImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		colourImageInfo.flags = 0; // Optional

		VulkanDevice::Get().CreateImageWithInfo(
			colourImageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			image.Image,
			image.ImageMemory);

		VkImageViewCreateInfo colourViewInfo{};
		colourViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colourViewInfo.image = image.Image;
		colourViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colourViewInfo.format = colourFormat;
		colourViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colourViewInfo.subresourceRange.baseMipLevel = 0;
		colourViewInfo.subresourceRange.levelCount = 1;
		colourViewInfo.subresourceRange.baseArrayLayer = 0;
		colourViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(
			device,
			&colourViewInfo,
			nullptr,
			&image.ImageView)
			!= VK_SUCCESS)
		{
			SL_ERROR("Failed to create texture image view!");
		}

		VkSamplerCreateInfo colourSamplerInfo{};
		colourSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		colourSamplerInfo.magFilter = VK_FILTER_LINEAR;
		colourSamplerInfo.minFilter = VK_FILTER_LINEAR;
		colourSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		colourSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		colourSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		colourSamplerInfo.anisotropyEnable = VK_TRUE;
		colourSamplerInfo.maxAnisotropy = VulkanDevice::Get().PhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		colourSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		colourSamplerInfo.unnormalizedCoordinates = VK_FALSE;
		colourSamplerInfo.compareEnable = VK_FALSE;
		colourSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		colourSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		colourSamplerInfo.mipLodBias = 0.0f;
		colourSamplerInfo.minLod = 0.0f;
		colourSamplerInfo.maxLod = 0.0f;

		vkCreateSampler(
			device,
			&colourSamplerInfo,
			nullptr,
			&image.Sampler);

		return image;
	}

	VulkanFramebuffer::Image VulkanFramebuffer::CreateDepthStencil(VkFormat depthStencilFormat)
	{

		auto device = VulkanDevice::Get().Device();

		VulkanFramebuffer::Image image;

		VkImageCreateInfo depthImageInfo{};
		depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
		depthImageInfo.extent.width = static_cast<uint32_t>(m_Width);
		depthImageInfo.extent.height = static_cast<uint32_t>(m_Height);
		depthImageInfo.extent.depth = 1;
		depthImageInfo.mipLevels = 1;
		depthImageInfo.arrayLayers = 1;
		depthImageInfo.format = depthStencilFormat;
		depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		depthImageInfo.flags = 0; // Optional

		VulkanDevice::Get().CreateImageWithInfo(
			depthImageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			image.Image,
			image.ImageMemory);

		VkImageViewCreateInfo depthViewInfo{};
		depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthViewInfo.image = image.Image;
		depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewInfo.format = depthStencilFormat;
		depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthViewInfo.subresourceRange.baseMipLevel = 0;
		depthViewInfo.subresourceRange.levelCount = 1;
		depthViewInfo.subresourceRange.baseArrayLayer = 0;
		depthViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(
			device,
			&depthViewInfo,
			nullptr,
			&image.ImageView)
			!= VK_SUCCESS)
		{
			SL_ERROR("Failed to create texture image view!");
		}

		VkSamplerCreateInfo depthSamplerInfo{};
		depthSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		depthSamplerInfo.magFilter = VK_FILTER_LINEAR;
		depthSamplerInfo.minFilter = VK_FILTER_LINEAR;
		depthSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		depthSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		depthSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		depthSamplerInfo.anisotropyEnable = VK_TRUE;
		depthSamplerInfo.maxAnisotropy = VulkanDevice::Get().PhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		depthSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		depthSamplerInfo.unnormalizedCoordinates = VK_FALSE;
		depthSamplerInfo.compareEnable = VK_FALSE;
		depthSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		depthSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		depthSamplerInfo.mipLodBias = 0.0f;
		depthSamplerInfo.minLod = 0.0f;
		depthSamplerInfo.maxLod = 0.0f;

		vkCreateSampler(
			device,
			&depthSamplerInfo,
			nullptr,
			&image.Sampler);

		return image;
	}

}