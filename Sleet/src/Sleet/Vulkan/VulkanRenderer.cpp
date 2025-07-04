#include "slpch.h"

#include "Sleet/Vulkan/VulkanRenderer.h"

#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/VulkanVertexBuffer.h"
#include "Sleet/Vulkan/VulkanIndexBuffer.h"
#include "Sleet/Vulkan/VulkanUniformBuffer.h"
#include "Sleet/Vulkan/VulkanDescriptorSet.h"
#include "Sleet/Vulkan/VulkanFramebuffer.h"

#include "Sleet/ImGui/imgui_impl_vulkan.h"

namespace Sleet {

	VulkanRenderer::VulkanRenderer()
	{
		auto device = VulkanDevice::Get().Device();

		m_Swapchain = CreateScope<VulkanSwapchain>();

		vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR"));
		vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR"));

		auto extent = m_Swapchain->GetExtent();
		SL_INFO("Swapchain extent {}, {}", extent.width, extent.height);

		// Create synchronization objects
		m_ImageAvailableSemaphores.resize(VulkanRenderer::MaxFramesInFlight);
		m_DrawFinishedSemaphores.resize(VulkanRenderer::MaxFramesInFlight);
		m_FrameInFlightFences.resize(VulkanRenderer::MaxFramesInFlight);
		m_ImageInFlightFences.resize(m_Swapchain->ImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < VulkanRenderer::MaxFramesInFlight; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_DrawFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &m_FrameInFlightFences[i]) != VK_SUCCESS)
			{
				SL_ERROR("Failed to create synchronization objects for a frame!");
			}
		}

		// Create command buffers
		m_CommandBuffers.resize(VulkanRenderer::MaxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VulkanDevice::Get().CommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(device, &allocInfo, m_CommandBuffers.data()) !=
			VK_SUCCESS)
		{
			SL_ERROR("Failed to allocate command buffers!");
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		auto device = VulkanDevice::Get().Device();

		vkDeviceWaitIdle(device);

		for (size_t i = 0; i < VulkanRenderer::MaxFramesInFlight; i++)
		{
			vkDestroySemaphore(device, m_DrawFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, m_FrameInFlightFences[i], nullptr);
		}
	}

	void VulkanRenderer::BeginFrame()
	{
		auto device = VulkanDevice::Get().Device();

		vkWaitForFences(
			device,
			1,
			&m_FrameInFlightFences[m_CurrentFrameIndex],
			VK_TRUE,
			UINT64_MAX);

		auto result = m_Swapchain->AquireNextImage(&m_CurrentImageIndex, m_ImageAvailableSemaphores[m_CurrentFrameIndex]);

		// Begin recording the command buffer
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrameIndex], &beginInfo) != VK_SUCCESS)
		{
			SL_ERROR("failed to begin recording command buffer!");
		}

	}

	void VulkanRenderer::BeginSwapchainRendering()
	{
		auto commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];

		VulkanDevice::Get().InsertImageMemoryBarrier(
			commandBuffer,
			m_Swapchain->GetImage(m_CurrentImageIndex),
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		/*
		VulkanDevice::Get().InsertImageMemoryBarrier(
			commandBuffer,
			m_Swapchain->GetDepthImage(m_CurrentFrameIndex),
			0,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 });
		*/

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(m_Swapchain->GetExtent().height);
		viewport.width = static_cast<float>(m_Swapchain->GetExtent().width);
		viewport.height = -static_cast<float>(m_Swapchain->GetExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor{ {0, 0}, m_Swapchain->GetExtent() };
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkRenderingAttachmentInfoKHR colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = m_Swapchain->GetImageView(m_CurrentImageIndex);
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.clearValue.color = { 0.1f, 0.1f, 0.7f, 0.0f };

		VkRenderingAttachmentInfoKHR depthStencilAttachment{};
		depthStencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthStencilAttachment.imageView = m_Swapchain->GetDepthView(m_CurrentFrameIndex);
		depthStencilAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthStencilAttachment.clearValue.depthStencil = { 1.0f,  0 };

		VkRenderingInfoKHR renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = { 0, 0, m_Swapchain->GetExtent().width, m_Swapchain->GetExtent().height };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;
		renderingInfo.pDepthAttachment = &depthStencilAttachment;
		renderingInfo.pStencilAttachment = nullptr;

		vkCmdBeginRenderingKHR(commandBuffer, &renderingInfo);
	}

	void VulkanRenderer::EndSwapchainRendering()
	{
		auto commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];

		vkCmdEndRenderingKHR(commandBuffer);

		// Transition the swapchain image for presentation

		VulkanDevice::Get().InsertImageMemoryBarrier(
			commandBuffer,
			m_Swapchain->GetImage(m_CurrentImageIndex),
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
	}

	void VulkanRenderer::BeginRendering(Ref<Framebuffer> framebuffer)
	{
		auto fb = std::static_pointer_cast<VulkanFramebuffer>(framebuffer);
		auto commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];

		VulkanDevice::Get().InsertImageMemoryBarrier(
			commandBuffer,
			fb->GetColourImage(0),
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(fb->GetHeight());
		viewport.width = static_cast<float>(fb->GetWidth());
		viewport.height = -static_cast<float>(fb->GetHeight());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor{ {0, 0}, {fb->GetWidth(), fb->GetHeight()}};
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		std::vector<VkRenderingAttachmentInfoKHR> colorAttachments;

		for (int i = 0; i < fb->ColourAttachmentCount(); i++)
		{
			VkRenderingAttachmentInfoKHR colorAttachment{};

			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.imageView = fb->GetColourView(i);
			colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.clearValue.color = { 0.7f, 0.1f, 0.1f, 1.0f };

			colorAttachments.push_back(colorAttachment);
		}

		VkRenderingAttachmentInfoKHR depthStencilAttachment{};
		depthStencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthStencilAttachment.imageView = fb->GetDepthView();
		depthStencilAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthStencilAttachment.clearValue.depthStencil = { 1.0f,  0 };

		VkRenderingInfoKHR renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = { 0, 0, fb->GetWidth(), fb->GetHeight()};
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = colorAttachments.size();
		renderingInfo.pColorAttachments = colorAttachments.data();
		renderingInfo.pDepthAttachment = &depthStencilAttachment;
		renderingInfo.pStencilAttachment = nullptr;

		vkCmdBeginRenderingKHR(commandBuffer, &renderingInfo);
	}

	void VulkanRenderer::EndRendering(Ref<Framebuffer> framebuffer)
	{
		auto commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];
		auto fb = std::static_pointer_cast<VulkanFramebuffer>(framebuffer);

		vkCmdEndRenderingKHR(commandBuffer);

		VulkanDevice::Get().InsertImageMemoryBarrier(
			commandBuffer,
			fb->GetColourImage(0),
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
	}

	void VulkanRenderer::EndFrame()
	{
		auto device = VulkanDevice::Get().Device();
		auto commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];

		// Finish and submit the command buffer to the graphics queue
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			SL_ERROR("Failed to record command buffer!");
		}

		if (m_ImageInFlightFences[m_CurrentImageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(device, 1, &m_ImageInFlightFences[m_CurrentImageIndex], VK_TRUE, UINT64_MAX);
		}
		m_ImageInFlightFences[m_CurrentImageIndex] = m_FrameInFlightFences[m_CurrentFrameIndex];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrameIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { m_DrawFinishedSemaphores[m_CurrentFrameIndex] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(device, 1, &m_FrameInFlightFences[m_CurrentFrameIndex]);
		if (vkQueueSubmit(VulkanDevice::Get().GraphicsQueue(), 1, &submitInfo, m_FrameInFlightFences[m_CurrentFrameIndex]) !=
			VK_SUCCESS)
		{
			SL_ERROR("Failed to submit draw command buffer!");
		}

		// Submit the swapchain image for presentation
		auto result = m_Swapchain->QueuePresent(&m_CurrentImageIndex, m_DrawFinishedSemaphores[m_CurrentFrameIndex]);

		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % VulkanRenderer::MaxFramesInFlight;
	}

	void VulkanRenderer::BindPipeline(Ref<Pipeline> pipeline)
	{
		m_LatestPipeline = std::static_pointer_cast<VulkanPipeline>(pipeline);
		m_LatestLayout = std::static_pointer_cast<VulkanPipeline>(pipeline)->GetPipelineLayout();

		auto commandBuffer = GetCurrentCommandBuffer();

		vkCmdBindPipeline(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_LatestPipeline->GetPipeline());

	}

	void VulkanRenderer::BindVertexBuffer(Ref<VertexBuffer> vertexBuffer)
	{
		auto commandBuffer = GetCurrentCommandBuffer();

		VkBuffer vertexBuffers[] = { std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer)->GetBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	void VulkanRenderer::BindIndexBuffer(Ref<IndexBuffer> indexBuffer)
	{
		auto commandBuffer = GetCurrentCommandBuffer();

		vkCmdBindIndexBuffer(commandBuffer, std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer)->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanRenderer::BindDescriptorSet(Ref<DescriptorSet> descriptorSet, uint32_t firstSet)
	{
		auto commandBuffer = GetCurrentCommandBuffer();
		auto set = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet)->GetDescriptorSet();

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_LatestLayout,
			firstSet,
			1,
			&set,
			0,
			nullptr
		);
	}

	void VulkanRenderer::DrawImGui()
	{
		auto commandBuffer = GetCurrentCommandBuffer();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	void VulkanRenderer::DrawIndexed(uint32_t indexCount, uint32_t firstIndex)
	{
		auto commandBuffer = GetCurrentCommandBuffer();
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, firstIndex, 0, 0);
	}

	void VulkanRenderer::SetTransform(glm::mat4 transform)
	{
		auto commandBuffer = GetCurrentCommandBuffer();

		vkCmdPushConstants(
			commandBuffer,
			m_LatestLayout,
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(glm::mat4),
			&transform);
	}

	void VulkanRenderer::WaitIdle()
	{
		vkDeviceWaitIdle(VulkanDevice::Get().Device());
	}

	void VulkanRenderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		m_Swapchain->RecreateSwapchain();
	}


}