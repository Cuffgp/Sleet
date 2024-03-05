#include "slpch.h"

#include "Sleet/Core/Application.h"

#include "Sleet/Vulkan/VulkanSwapchain.h"

namespace Sleet {

	VkFormat VulkanSwapchain::s_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkFormat VulkanSwapchain::s_SwapchainDepthFormat = VK_FORMAT_UNDEFINED;

	VulkanSwapchain::VulkanSwapchain()
	{
		CreateSurface();
		CreateSwapchain();
		CreateImages();
		CreateDepthResources();
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		auto device = VulkanDevice::Get().Device();
		auto instance = VulkanDevice::Get().Instance();

		CleanupSwapchain();

		vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
		vkDestroySurfaceKHR(instance, m_Surface, nullptr);
	}

	void VulkanSwapchain::CleanupSwapchain()
	{
		auto device = VulkanDevice::Get().Device();

		for (auto imageView : m_SwapchainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		for (int i = 0; i < m_DepthImages.size(); i++)
		{
			vkDestroyImageView(device, m_DepthImageViews[i], nullptr);
			vkDestroyImage(device, m_DepthImages[i], nullptr);
			vkFreeMemory(device, m_DepthImageMemorys[i], nullptr);
		}

		vkDestroySwapchainKHR(device, m_OldSwapchain, nullptr);
		m_OldSwapchain = m_Swapchain;
	}

	void VulkanSwapchain::RecreateSwapchain()
	{
		auto device = VulkanDevice::Get().Device();

		vkDeviceWaitIdle(device);
		CleanupSwapchain();
		CreateSwapchain();
		CreateImages();
		CreateDepthResources();
	}

	void VulkanSwapchain::CreateSurface()
	{
		auto window = Application::Get().GetWindow().GetWindowPointer();
		auto instance = VulkanDevice::Get().Instance();

		if (glfwCreateWindowSurface(instance, window, nullptr, &m_Surface) != VK_SUCCESS) 
		{
			SL_ERROR("Failed to create window surface!");
		}
	}

	void VulkanSwapchain::CreateSwapchain()
	{
		auto device = VulkanDevice::Get().Device();

		QuerySwapchainSupport();
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(m_SupportDetails.Formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(m_SupportDetails.PresentModes);
		m_Extent = ChooseSwapExtent(m_SupportDetails.Capabilities);

		//SL_INFO("Swapchain Extent {} {}", m_Extent.width, m_Extent.height);

		m_ImageFormat = surfaceFormat.format;
		m_DepthFormat = VulkanDevice::Get().FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		s_SwapchainImageFormat = m_ImageFormat;
		s_SwapchainDepthFormat = m_DepthFormat;

		uint32_t imageCount = m_SupportDetails.Capabilities.minImageCount + 1;
		if (m_SupportDetails.Capabilities.maxImageCount > 0 && imageCount > m_SupportDetails.Capabilities.maxImageCount)
		{
			imageCount = m_SupportDetails.Capabilities.maxImageCount;
		}

		//SL_INFO("Format {}, Colorspace {}", string_VkFormat(surfaceFormat.format), string_VkColorSpaceKHR(surfaceFormat.colorSpace));

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_ImageFormat;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
		createInfo.preTransform = m_SupportDetails.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = m_OldSwapchain == nullptr ? nullptr : m_OldSwapchain;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS) 
		{
			SL_ERROR("Failed to create swap chain!");
		}
	}

	void VulkanSwapchain::CreateImages()
	{
		auto device = VulkanDevice::Get().Device();

		vkGetSwapchainImagesKHR(device, m_Swapchain, &m_SwapchainImageCount, nullptr);
		m_SwapchainImages.resize(m_SwapchainImageCount);
		vkGetSwapchainImagesKHR(device, m_Swapchain, &m_SwapchainImageCount, m_SwapchainImages.data());

		m_SwapchainImageViews.resize(m_SwapchainImageCount);

		for (size_t i = 0; i < m_SwapchainImageCount; i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_ImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &m_SwapchainImageViews[i]) != VK_SUCCESS) 
			{
				SL_ERROR("Failed to create image views!");
			}
		}
	}

	void VulkanSwapchain::CreateDepthResources()
	{
		auto device = VulkanDevice::Get().Device();

		m_DepthImages.resize(m_SwapchainImageCount);
		m_DepthImageMemorys.resize(m_SwapchainImageCount);
		m_DepthImageViews.resize(m_SwapchainImageCount);

		for (int i = 0; i < m_DepthImages.size(); i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = m_Extent.width;
			imageInfo.extent.height = m_Extent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_DepthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			VulkanDevice::Get().CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_DepthImages[i],
				m_DepthImageMemorys[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_DepthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_DepthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &viewInfo, nullptr, &m_DepthImageViews[i]) != VK_SUCCESS)
			{
				SL_ERROR("Failed to create texture image view!");
			}
		}
	}

	VkResult VulkanSwapchain::AquireNextImage(uint32_t* imageIndex, VkSemaphore imageAvalible)
	{
		auto device = VulkanDevice::Get().Device();

		VkResult result = vkAcquireNextImageKHR(
			device,
			m_Swapchain,
			UINT64_MAX,
			imageAvalible,  // must be a not signaled semaphore
			VK_NULL_HANDLE,
			imageIndex);

		return result;
	}

	VkResult VulkanSwapchain::QueuePresent(uint32_t* imageIndex, VkSemaphore renderFinished)
	{
		VkSwapchainKHR swapChains[] = { m_Swapchain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinished;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = imageIndex;

		auto result = vkQueuePresentKHR(VulkanDevice::Get().GraphicsQueue(), &presentInfo);

		return result;
	}

	void VulkanSwapchain::QuerySwapchainSupport()
	{
		auto physicalDevice = VulkanDevice::Get().PhysicalDevice();

		// Get the surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &m_SupportDetails.Capabilities);

		// Get the surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);
		if (formatCount != 0) 
		{
			m_SupportDetails.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, m_SupportDetails.Formats.data());
		}

		// Get the present modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) 
		{
			m_SupportDetails.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, m_SupportDetails.PresentModes.data());
		}
	}

	VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats)
	{
		//SL_TRACE("Surface formats avalible:");
		VkSurfaceFormatKHR format = formats[0];
		for (const auto& availableFormat : formats) 
		{
			//SL_TRACE("	{}, {}", string_VkFormat(availableFormat.format), string_VkColorSpaceKHR(availableFormat.colorSpace));

			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
			{
				format = availableFormat;
			}
		}
		return format;
	}

	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(std::vector<VkPresentModeKHR> presentModes)
	{
		//SL_TRACE("Swapchain present modes avalible:");
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : presentModes) 
		{
			//SL_TRACE("	{}", string_VkPresentModeKHR(availablePresentMode));

			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				presentMode = availablePresentMode;
			}
		}
		return presentMode;
	}

	VkExtent2D VulkanSwapchain::ChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities)
	{
		auto window = Application::Get().GetWindow().GetWindowPointer();

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		VkExtent2D windowExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = windowExtent;
			actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}

}