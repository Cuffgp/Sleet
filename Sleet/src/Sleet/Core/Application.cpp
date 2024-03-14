#include "slpch.h"

#include "Sleet/Core/Utilities.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Renderer/Renderer.h"
#include "Sleet/Renderer/Camera.h"

#include "Sleet/Renderer/GltfLoader.h"

namespace Sleet {
	
	Application* Application::s_Instance = nullptr;

	struct SceneUBO
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec4 AmbientLight;
		glm::vec4 LightPosition;
		glm::vec4 LightColour;
	};

	Application::Application()
	{
		s_Instance = this;

		SL_INFO("Creating application");

		m_Window = CreateScope<Window>(1280, 720, "Sleet Application");
		m_Window->SetResizeCallback(SL_BIND_FN(Application::OnWindowResize));

		Renderer::Init();

		float CubeVertices[] = {
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f
		};

		uint32_t CubeIndices[] = {
			0, 1, 2,  2, 3, 0,
			1, 5, 6,  6, 2, 1,
			5, 4, 7,  7, 6, 5,
			4, 0, 3,  3, 7, 4,
			3, 2, 6,  6, 7, 3,
			4, 5, 1,  1, 0, 4
		};

		m_CubeVertex = VertexBuffer::Create(CubeVertices, 3 * 8 * sizeof(float));
		m_CubeIndex = IndexBuffer::Create(CubeIndices, 6 * 6 * sizeof(uint32_t));

		GltfLoader mesh = GltfLoader("assets/gltf/cube/cube.gltf");

		m_VertexBuffer = mesh.GetVertexBuffer();
		m_IndexBuffer = mesh.GetIndexBuffer();

		m_Texture = Texture::Create("assets/gltf/cube/Cube_BaseColor.png");

		auto vertexInput = VertexInput({
		{ "Pos",      ShaderDataType::Float3, 0 },
		{ "Normal",   ShaderDataType::Float3, 0 },
		{ "Uv",       ShaderDataType::Float2, 0 }
			});

		DescriptorMap sceneMap;
		sceneMap[0] = ShaderDescriptor(0, 0, ShaderStage::All, ShaderDescriptorType::Uniform, 1);

		for (int i = 0; i < Renderer::FramesInFlight(); i++)
		{
			m_SceneDescriptorSet.push_back(DescriptorSet::Create(sceneMap));
			m_SceneUniform.push_back(UniformBuffer::Create(sizeof(SceneUBO)));

			m_SceneUniform[i]->Write(m_SceneDescriptorSet[i], 0);
		}

		DescriptorMap constantMap;
		constantMap[0] = ShaderDescriptor(1, 0, ShaderStage::Fragment, ShaderDescriptorType::ImageSampler, 1);

		m_ConstantSet = DescriptorSet::Create(constantMap);
		m_Texture->Write(m_ConstantSet, 0);

		m_MeshPipeline = Pipeline::Create("assets/shaders/mesh_shader");
		m_SimplePipeline = Pipeline::Create("assets/shaders/transform_shader");

		SL_INFO("Index Count {}", m_IndexBuffer->GetIndexCount());

	}
	
	Application::~Application()
	{
		Renderer::Shutdown();
		SL_INFO("Destroying application");
	}

	void Application::Run()
	{
		SceneUBO sceneUBO;

		Camera camera(glm::vec3{ 0., 0., -9. }, glm::vec3{ 0., 0., 1. });

		while (!m_Window->ShouldClose())
		{
			float time = (float)glfwGetTime();
			float timestep = time - m_LastTime;
			m_LastTime = time;

			camera.OnUpdate(timestep);

			sceneUBO.View = camera.GetView();
			sceneUBO.Projection = camera.GetProjection();

			//float intensity = std::sin(time)*std::sin(time);
			float intensity = 0.0f;

			sceneUBO.AmbientLight = glm::vec4(intensity, intensity, intensity, 1.0);
			sceneUBO.LightPosition = glm::vec4(0., -4., 0., 0.);
			sceneUBO.LightColour = glm::vec4(0., 1., 0., 0.);

			m_SceneUniform[Renderer::CurrentFrameIndex()]->SetData(&sceneUBO);

			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3{ 2, 0, 0 });
			glm::mat4 translation2 = glm::translate(glm::mat4(1.0f), glm::vec3{ -2, 0, 0 });
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1, 1, 1));
			glm::mat4 rotation2 = glm::rotate(glm::mat4(1.0f), time, glm::vec3(-0.8, -0.5, 1));

			glm::mat4 cubeTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(sceneUBO.LightPosition));

			m_Window->PollEvents();

			Renderer::BeginFrame(m_SimplePipeline);
			Renderer::BindVertexBuffer(m_CubeVertex);
			Renderer::BindIndexBuffer(m_CubeIndex);
			Renderer::SetTransform(sceneUBO.Projection * sceneUBO.View * cubeTranslation);
			Renderer::DrawIndexed(m_CubeIndex->GetIndexCount());

			Renderer::BindPipeline(m_MeshPipeline);
			Renderer::BindVertexBuffer(m_VertexBuffer);
			Renderer::BindIndexBuffer(m_IndexBuffer);

			Renderer::BindDescriptorSet(m_SceneDescriptorSet[Renderer::CurrentFrameIndex()], 0);
			Renderer::BindDescriptorSet(m_ConstantSet, 1);

			Renderer::SetTransform(translation * rotation);
			Renderer::DrawIndexed(m_IndexBuffer->GetIndexCount());
			Renderer::SetTransform(translation2 * rotation2);
			Renderer::DrawIndexed(m_IndexBuffer->GetIndexCount());
			Renderer::EndFrame();
		}
	}

	void Application::OnWindowResize(uint32_t width, uint32_t height)
	{
		SL_INFO("Window resizing");
		Renderer::OnWindowResize(width, height);
	}
	
}