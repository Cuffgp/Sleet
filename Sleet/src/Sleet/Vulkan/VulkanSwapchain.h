#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"

namespace Sleet {

	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain();
		~VulkanSwapchain();

		VkExtent2D GetExtent() { return m_Extent; }
		VkFormat GetImageFormat() { return m_ImageFormat; }
		uint32_t ImageCount() { return m_SwapchainImageCount; }
		VkImageView GetImageView(uint32_t index) { return m_SwapchainImageViews[index]; }
		VkImageView GetDepthView(uint32_t index) { return m_DepthImageViews[index]; }
		VkImage GetImage(uint32_t index) { return m_SwapchainImages[index]; }
		VkImage GetDepthImage(uint32_t index) { return m_DepthImages[index]; }

		VkResult AquireNextImage(uint32_t* imageIndex, VkSemaphore imageAvalible);
		VkResult QueuePresent(uint32_t* imageIndex, VkSemaphore imageAvalible);

		void RecreateSwapchain();

		static inline VkFormat s_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
		static inline VkFormat s_SwapchainDepthFormat = VK_FORMAT_UNDEFINED;
	private:
		void CleanupSwapchain();
		void CreateSurface();
		void CreateSwapchain();
		void CreateImages();
		void CreateDepthResources();

		void QuerySwapchainSupport();
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats);
		VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR> presentModes);
		VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);
	private:
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_Swapchain;
		VkSwapchainKHR m_OldSwapchain = nullptr;
		SwapChainSupportDetails m_SupportDetails;
		VkExtent2D m_Extent;
		VkFormat m_ImageFormat;
		VkFormat m_DepthFormat;

		uint32_t m_SwapchainImageCount;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemorys;
		std::vector<VkImageView> m_DepthImageViews;
	};

}