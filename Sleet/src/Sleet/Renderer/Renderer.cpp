#include "slpch.h"

#include "Sleet/Renderer/Renderer.h"

namespace Sleet {

	void Renderer::Init()
	{
		s_Renderer = RendererAPI::Create();
	}

	void Renderer::Shutdown()
	{
		s_Renderer->WaitIdle();
		delete s_Renderer;
	}

	void Renderer::BeginFrame()
	{
		s_Renderer->BeginFrame();
		s_Renderer->BeginSwapchainRendering();
	}

	void Renderer::EndFrame()
	{
		s_Renderer->EndSwapchainRendering();
		s_Renderer->EndFrame();
	}

	void Renderer::WaitIdle()
	{
		s_Renderer->WaitIdle();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_Renderer->OnWindowResize(width, height);
	}

	void Renderer::BindPipeline(Ref<Pipeline> pipeline)
	{
		s_Renderer->BindPipeline(pipeline);
	}

	void Renderer::BindVertexBuffer(Ref<VertexBuffer> vertexBuffer)
	{
		s_Renderer->BindVertexBuffer(vertexBuffer);
	}

	void Renderer::BindIndexBuffer(Ref<IndexBuffer> indexBuffer)
	{
		s_Renderer->BindIndexBuffer(indexBuffer);
	}

	void Renderer::BindDescriptorSet(Ref<DescriptorSet> descriptorSet, uint32_t firstSet)
	{
		s_Renderer->BindDescriptorSet(descriptorSet, firstSet);
	}

	void Renderer::DrawImGui()
	{
		s_Renderer->DrawImGui();
	}

	void Renderer::DrawIndexed(uint32_t indexCount, uint32_t firstIndex)
	{
		s_Renderer->DrawIndexed(indexCount, firstIndex);
	}

	void Renderer::SetTransform(glm::mat4 transform)
	{
		s_Renderer->SetTransform(transform);
	}

	uint32_t Renderer::FramesInFlight()
	{
		return s_Renderer->FramesInFlight();
	}

	uint32_t Renderer::CurrentFrameIndex()
	{
		return s_Renderer->CurrentFrameIndex();
	}

}