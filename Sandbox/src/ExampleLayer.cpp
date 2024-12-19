#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

ExampleLayer::ExampleLayer()
{
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

		m_CubeVertex = Sleet::VertexBuffer::Create(CubeVertices, 3 * 8 * sizeof(float));
		m_CubeIndex = Sleet::IndexBuffer::Create(CubeIndices, 6 * 6 * sizeof(uint32_t));

		//GltfLoader mesh = GltfLoader("assets/gltf/lantern/lantern.gltf");
		//ObjLoader mesh = ObjLoader("assets/well/well.obj");

		//m_VertexBuffer = mesh.GetVertexBuffer();
		//m_IndexBuffer = mesh.GetIndexBuffer();

		m_Texture = Sleet::Texture::Create("assets/gltf/lantern/Lantern_baseColor.png");

		auto vertexInput = Sleet::VertexInput({
		{ "Pos",      Sleet::ShaderDataType::Float3, 0 },
		{ "Normal",   Sleet::ShaderDataType::Float3, 0 },
		{ "Uv",       Sleet::ShaderDataType::Float2, 0 }
			});

		Sleet::DescriptorMap sceneMap;
		sceneMap[0] = Sleet::ShaderDescriptor(0, 0, Sleet::ShaderStage::All, Sleet::ShaderDescriptorType::Uniform, 1);

		for (int i = 0; i < Sleet::Renderer::FramesInFlight(); i++)
		{
			m_SceneDescriptorSet.push_back(Sleet::DescriptorSet::Create(sceneMap));
			m_SceneUniform.push_back(Sleet::UniformBuffer::Create(sizeof(SceneUBO)));

			m_SceneUniform[i]->Write(m_SceneDescriptorSet[i], 0);
		}

		Sleet::DescriptorMap constantMap;
		constantMap[0] = Sleet::ShaderDescriptor(1, 0, Sleet::ShaderStage::Fragment, Sleet::ShaderDescriptorType::ImageSampler, 1);

		m_ConstantSet = Sleet::DescriptorSet::Create(constantMap);
		m_Texture->Write(m_ConstantSet, 0);

		m_MeshPipeline = Sleet::Pipeline::Create("assets/shaders/mesh_shader");
		m_SimplePipeline = Sleet::Pipeline::Create("assets/shaders/transform_shader");

		m_Camera = Sleet::Camera(glm::vec3{ 0., 0., -40. }, glm::vec3{ 0., 0., 1. });

		//SL_INFO("Index Count {}", m_IndexBuffer->GetIndexCount());
		time = 0;
}


void ExampleLayer::OnAttach()
{

}

void ExampleLayer::OnDetach()
{

}

void ExampleLayer::OnUpdate(float ts)
{
	m_Camera.OnUpdate(ts);
	time += 1. / 60.;

	m_SceneUBO.View = m_Camera.GetView();
	m_SceneUBO.Projection = m_Camera.GetProjection();

	//float intensity = std::sin(time)*std::sin(time);
	float intensity = 0.5f;

	m_SceneUBO.AmbientLight = glm::vec4(intensity, intensity, intensity, 1.0);
	m_SceneUBO.LightPosition = glm::vec4(5 * std::cos(time), 5 * std::sin(time), 0., 0.);
	m_SceneUBO.LightColour = glm::vec4(0.2, 0.6, 1., 0.);

	m_SceneUniform[Sleet::Renderer::CurrentFrameIndex()]->SetData(&m_SceneUBO);

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3{ 2, 0, 0 });
	glm::mat4 translation2 = glm::translate(glm::mat4(1.0f), glm::vec3{ -2, 0, 0 });

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1, 1, 1));
	glm::mat4 rotation2 = glm::rotate(glm::mat4(1.0f), time, glm::vec3(-0.8, -0.5, 1));

	//glm::mat4 cubeTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(sceneUBO.LightPosition));
	glm::mat4 cubeTranslation = glm::mat4(1.0f);

	Sleet::Renderer::BindVertexBuffer(m_CubeVertex);
	Sleet::Renderer::BindIndexBuffer(m_CubeIndex);
	Sleet::Renderer::SetTransform(m_SceneUBO.Projection * m_SceneUBO.View * cubeTranslation);
	Sleet::Renderer::DrawIndexed(m_CubeIndex->GetIndexCount());

	//Renderer::BindPipeline(m_MeshPipeline);
	//Renderer::BindVertexBuffer(m_VertexBuffer);
	//Renderer::BindIndexBuffer(m_IndexBuffer);

	//Renderer::BindDescriptorSet(m_SceneDescriptorSet[Renderer::CurrentFrameIndex()], 0);
	//Renderer::BindDescriptorSet(m_ConstantSet, 1);

	//Renderer::SetTransform(translation);
	//Renderer::DrawIndexed(m_IndexBuffer->GetIndexCount());
	//Renderer::SetTransform(translation2 * rotation2);
	//Renderer::DrawIndexed(m_IndexBuffer->GetIndexCount());
}

void ExampleLayer::OnImGuiRender()
{
	
}