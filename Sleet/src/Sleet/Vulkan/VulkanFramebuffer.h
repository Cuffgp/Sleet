#pragma once

#include "Sleet/Renderer/Framebuffer.h"
#include "Sleet/Renderer/Renderer.h"

#include "Sleet/Vulkan/VulkanDevice.h"

namespace Sleet {

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		struct Image
		{
			VkImage Image;
			VkDeviceMemory ImageMemory;
			VkImageView ImageView;
			VkSampler Sampler;
		};

		struct Attachments
		{
			std::vector<Image> ColourAttachments;
			Image DepthStencilAttachment;
		};

	public:
		VulkanFramebuffer(uint32_t width, uint32_t height);
		VulkanFramebuffer(uint32_t width, uint32_t height, std::vector<ImageFormat> attachments);

		~VulkanFramebuffer();

		virtual void Write(Ref<DescriptorSet> descriptorSet, uint32_t binding, uint32_t frameIndex) override;

		VkPipelineRenderingCreateInfo GetPipelineRenderingInfo();

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

		uint32_t ColourAttachmentCount() { return m_ColourFormats.size(); }
		

		VkImageView GetColourView(uint32_t i) { return m_Attachments[Renderer::CurrentFrameIndex()].ColourAttachments[i].ImageView; }
		VkImageView GetDepthView() { return m_Attachments[Renderer::CurrentFrameIndex()].DepthStencilAttachment.ImageView; }

		VkImage GetColourImage(uint32_t i) { return m_Attachments[Renderer::CurrentFrameIndex()].ColourAttachments[i].Image; }
		VkImage GetDepthImage() { return m_Attachments[Renderer::CurrentFrameIndex()].DepthStencilAttachment.Image; }

	private:
		Image CreateColourAttachment(VkFormat colourFormat);
		Image CreateDepthStencil(VkFormat depthStencilFormat);
	private:

		uint32_t m_Width;
		uint32_t m_Height;
		std::vector<Attachments> m_Attachments;

		std::vector<VkFormat> m_ColourFormats;
		VkFormat m_DepthFormat;
	};

}