#pragma once

#include "glm/glm.hpp"

#include "Sleet/Renderer/Pipeline.h"
#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"
#include "Sleet/Renderer/UniformBuffer.h"
#include "Sleet/Renderer/DescriptorSet.h"

namespace Sleet {

	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginSwapchainRendering() = 0;
		virtual void EndSwapchainRendering() = 0;

		virtual void WaitIdle() = 0;
		virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

		virtual void BindPipeline(Ref<Pipeline> pipeline) = 0;
		virtual void BindVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;
		virtual void BindIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;
		virtual void BindDescriptorSet(Ref<DescriptorSet> descriptorSet, uint32_t firstSet) = 0;
		virtual void DrawImGui() = 0;

		virtual void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0) = 0;

		virtual void SetTransform(glm::mat4 transform) = 0;

		virtual uint32_t FramesInFlight() = 0;
		virtual uint32_t CurrentFrameIndex() = 0;

		static RendererAPI* Create();
	private:

	};


}