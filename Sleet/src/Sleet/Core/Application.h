#pragma once

#include "Sleet/Core/Base.h"
#include "Sleet/Core/Window.h"

#include "Sleet/Core/LayerStack.h"

#include "Sleet/Renderer/RendererAPI.h"

#include "Sleet/Renderer/Pipeline.h"
#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"
#include "Sleet/Renderer/UniformBuffer.h"
#include "Sleet/Renderer/DescriptorSet.h"
#include "Sleet/Renderer/Texture.h"

#include "Sleet/ImGui/ImGuiLayer.h"

namespace Sleet {
	
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }
		void Run();

	private:
		void OnWindowResize(uint32_t width, uint32_t height);
	private:
		Scope<Window> m_Window;
		float m_LastTime = 0;
		bool m_Running;
		bool m_Minimized;
		LayerStack m_LayerStack;

		ImGuiLayer* m_ImGuiLayer;

		static Application* s_Instance;
	};

	Application* CreateApplication();
}