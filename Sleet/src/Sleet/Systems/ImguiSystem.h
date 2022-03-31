#pragma once

#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanDevice.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Sleet {

	static void check_vk_result(VkResult err) 
	{
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class ImguiSystem
	{
	public:
		ImguiSystem(Window &window, VulkanDevice &device, VkRenderPass renderPass, uint32_t imageCount);
		~ImguiSystem();

		void beginFrame();
		void render(VkCommandBuffer commandBuffer);

		void example();

	private:
		VulkanDevice &device;
		VkDescriptorPool descriptorPool;

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};


}