#include "slpch.h"

#include "Sleet/ImGui/ImGuiLayer.h"

#include "Sleet/Core/Application.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanSwapchain.h"

#include "Sleet/Vulkan/VulkanImGuiLayer.h"

namespace Sleet {

	ImGuiLayer* ImGuiLayer::Create()
	{
		return new VulkanImGuiLayer;
	}


}