#include "slpch.h"

#include "Sleet/Core/Utilities.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Renderer/Renderer.h"
#include "Sleet/Renderer/Camera.h"

#include "Sleet/Renderer/GltfLoader.h"
#include "Sleet/Renderer/ObjLoader.h"

#include "Sleet/ImGui/ImGuiLayer.h"


namespace Sleet {
	
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;

		SL_INFO("Creating application");

		m_Window = CreateScope<Window>(1280, 720, "Sleet Application");
		m_Window->SetResizeCallback(SL_BIND_FN(Application::OnWindowResize));

		Renderer::Init();

		m_ImGuiLayer = ImGuiLayer::Create();
		PushOverlay(m_ImGuiLayer);

	}
	
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	Application::~Application()
	{
		Renderer::Shutdown();
		SL_INFO("Destroying application");
	}

	void Application::Run()
	{

		while (!m_Window->ShouldClose())
		{
			ExecuteQueue();

			m_Window->PollEvents();

			float time = (float)glfwGetTime();
			float timestep = time - m_LastTime;
			m_LastTime = time;

			m_ImGuiLayer->Begin();
			Renderer::BeginFrame();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			Renderer::EndFrame();

		}
	}

	void Application::SubmitToQueue(const std::function<void()>& function)
	{
		m_StartOfFrameQueue.emplace_back(function);
	}

	void Application::OnWindowResize(uint32_t width, uint32_t height)
	{
		SL_INFO("Window resizing");
		Renderer::OnWindowResize(width, height);
	}

	void Application::ExecuteQueue()
	{

		for (auto& func : m_StartOfFrameQueue)
			func();

		m_StartOfFrameQueue.clear();
	}
	
}