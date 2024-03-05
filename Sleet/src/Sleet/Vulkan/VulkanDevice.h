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
		VkCommandPool CommandPool() { return m_CommandPool; }
		VkDescriptorPool DescriptorPool() { return m_DescriptorPool; }

		static inline VulkanDevice& Get() { return *s_Device; }

		// Helper functions
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
			VkImageTiling tiling, VkFormatFeatureFlags features);
		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void CreateImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);
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