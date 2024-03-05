#include "Sleet/Core/Base.h"
#include "Sleet/Core/Window.h"

#include "Sleet/Renderer/RendererAPI.h"

#include "Sleet/Renderer/Pipeline.h"
#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"
#include "Sleet/Renderer/UniformBuffer.h"
#include "Sleet/Renderer/DescriptorSet.h"

namespace Sleet {
	
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		void OnWindowResize(uint32_t width, uint32_t height);

		inline Window& GetWindow() { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }
	private:
		Scope<Window> m_Window;
		float m_LastTime = 0;


		Ref<Pipeline> m_Pipeline;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<UniformBuffer> m_UniformBuffer;
		Ref<DescriptorSet> m_ConstantSet;

		static Application* s_Instance;
	};

	Application* CreateApplication();
}