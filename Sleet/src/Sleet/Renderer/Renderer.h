#pragma once

#include "Sleet/Renderer/RendererAPI.h"

namespace Sleet {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static inline RendererAPI& Get() { return *s_Renderer; }

		static void BeginFrame();
		static void EndFrame();

		static void WaitIdle();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BindPipeline(Ref<Pipeline> pipeline);
		static void BindVertexBuffer(Ref<VertexBuffer> vertexBuffer);
		static void BindIndexBuffer(Ref<IndexBuffer> indexBuffer);
		static void BindDescriptorSet(Ref<DescriptorSet> descriptorSet, uint32_t firstSet);
		static void DrawImGui();
		static void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0);

		static void SetTransform(glm::mat4 transform);

		static uint32_t FramesInFlight();
		static uint32_t CurrentFrameIndex();

	private:
		static inline RendererAPI* s_Renderer = nullptr;
	};

}