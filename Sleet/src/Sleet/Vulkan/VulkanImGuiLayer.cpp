#include "slpch.h"

#include "Sleet/Vulkan/VulkanImGuiLayer.h"

#include "Sleet/Core/Application.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanSwapchain.h"
#include "Sleet/Renderer/Renderer.h"

#include "Sleet/ImGui/imgui_impl_glfw.h"
#include "Sleet/ImGui/imgui_impl_vulkan.h"

#include "implot/implot.h"

namespace Sleet {

	VulkanImGuiLayer::VulkanImGuiLayer()
	{

	}

	void VulkanImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		auto window = Sleet::Application::Get().GetWindow().GetWindowPointer();

		VkPipelineRenderingCreateInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachmentFormats = &VulkanSwapchain::s_SwapchainImageFormat;
		renderingInfo.depthAttachmentFormat = VulkanSwapchain::s_SwapchainDepthFormat;
		renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = VulkanDevice::Get().Instance();
		init_info.PhysicalDevice = VulkanDevice::Get().PhysicalDevice();
		init_info.Device = VulkanDevice::Get().Device();
		init_info.QueueFamily = VulkanDevice::Get().GraphicsFamily();
		init_info.Queue = VulkanDevice::Get().GraphicsQueue();
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = VulkanDevice::Get().DescriptorPool();
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.CheckVkResultFn = nullptr;
		init_info.UseDynamicRendering = true;
		init_info.PipelineRenderingCreateInfo = renderingInfo;
		init_info.RenderPass = nullptr;
		ImGui_ImplVulkan_Init(&init_info);

		ImGui_ImplVulkan_CreateFontsTexture();

	}

	void VulkanImGuiLayer::OnDetach()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void VulkanImGuiLayer::OnImGuiRender()
	{

	}

	void VulkanImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::End()
	{
		ImGui::Render();
		Renderer::DrawImGui();
	}

}