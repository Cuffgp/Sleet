#pragma once

#include <vector>

#include "Vulkan.h"

namespace Sleet {

	struct QueueFamilyIndices
	{
		uint32_t Graphics = UINT32_MAX;
		uint32_t Compute = UINT32_MAX;
		uint32_t Transfer = UINT32_MAX;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator=(const VulkanDevice&) = delete;

		static void VulkanDevice::Init();
		static void VulkanDevice::Destroy();

		VkInstance Instance() { return m_Instance; }
		VkPhysicalDevice PhysicalDevice() { return m_PhysicalDevice; }
		VkDevice Device() { return m_Device; }
		VkQueue GraphicsQueue() { return m_GraphicsQueue; }
		uint32_t GraphicsFamily() { return m_QueueFamilyIndices.Graphics; }
		VkCommandPool CommandPool() { return m_CommandPool; }
		VkDescriptorPool DescriptorPool() { return m_DescriptorPool; }
		VkPhysicalDeviceProperties PhysicalDeviceProperties() { return m_PhysicalDeviceProperties; }

		static inline VulkanDevice& Get() { return *s_Device; }

		// Helper functions
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
			VkImageTiling tiling, VkFormatFeatureFlags features);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void CreateImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);

		void SetImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		void SetImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		void InsertImageMemoryBarrier(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresourceRange);
	private:
		VulkanDevice();
		~VulkanDevice();

		void CreateInstance();
		void SetupDebugMessenger();
		void CreatePhysicalDevice();
		void CreateDevice();

	private:
		static const bool m_Validation = true;
		VkInstance m_Instance;
		std::vector<VkExtensionProperties> m_InstanceExtensions;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkPhysicalDevice m_PhysicalDevice;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
		QueueFamilyIndices m_QueueFamilyIndices;
		std::vector<VkExtensionProperties> m_DeviceExtensions;

		VkDevice m_Device;
		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;
		VkQueue m_TransferQueue;
		VkCommandPool m_CommandPool;
		VkDescriptorPool m_DescriptorPool;

		static VulkanDevice* s_Device;
	};


}