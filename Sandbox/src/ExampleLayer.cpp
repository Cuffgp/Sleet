#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>



ExampleLayer::ExampleLayer()
{

	float QuadVertices[] = {
	-0.5f, -0.5f, 1.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 0.0f,
	 0.5f,  0.5f, 0.0f, 1.0f,
	-0.5f,  0.5f, 1.0f, 1.0f
	};

	uint32_t QuadIndices[] = {
		0, 1, 2, 2, 3, 0
	};

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

	m_QuadVertex = Sleet::VertexBuffer::Create(QuadVertices, 4 * 4 * sizeof(float));
	m_QuadIndex = Sleet::IndexBuffer::Create(QuadIndices, 6 * sizeof(uint32_t));

	m_CubeVertex = Sleet::VertexBuffer::Create(CubeVertices, 3 * 8 * sizeof(float));
	m_CubeIndex = Sleet::IndexBuffer::Create(CubeIndices, 6 * 6 * sizeof(uint32_t));

	m_Texture = Sleet::Texture::Create("assets/texture/WoodFloor.png");

	m_SimplePipeline = Sleet::Pipeline::Create("assets/shaders/quad_shader");

	m_Framebuffer = Sleet::Framebuffer::Create(200, 200);

	auto vertexInput = Sleet::VertexInput({
	{ "Pos",      Sleet::ShaderDataType::Float3, 0 },
	{ "Normal",   Sleet::ShaderDataType::Float3, 0 },
	{ "Uv",       Sleet::ShaderDataType::Float2, 0 }
		});

	Sleet::DescriptorMap sceneMap;
	sceneMap[0] = Sleet::ShaderDescriptor(0, 0, Sleet::ShaderStage::All, Sleet::ShaderDescriptorType::Uniform, 1);

	Sleet::DescriptorMap quadMap;
	quadMap[0] = Sleet::ShaderDescriptor(0, 0, Sleet::ShaderStage::Fragment, Sleet::ShaderDescriptorType::ImageSampler, 1);

	m_TextureSet = Sleet::DescriptorSet::Create(quadMap);
	m_Texture->Write(m_TextureSet, 0);

	for (int i = 0; i < Sleet::Renderer::FramesInFlight(); i++)
	{

		m_FramebufferDescriptorSet.push_back(Sleet::DescriptorSet::Create(quadMap));

	}

	for (int i = 0; i < Sleet::Renderer::FramesInFlight(); i++)
	{
		m_Framebuffer->Write(m_FramebufferDescriptorSet[i], 0, i);
	}

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

	//m_SceneUniform[Sleet::Renderer::CurrentFrameIndex()]->SetData(&m_SceneUBO);

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3{ 2, 0, 0 });
	glm::mat4 translation2 = glm::translate(glm::mat4(1.0f), glm::vec3{ -2, 0, 0 });

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1, 1, 1));
	glm::mat4 rotation2 = glm::rotate(glm::mat4(1.0f), time, glm::vec3(-0.8, -0.5, 1));

	//glm::mat4 cubeTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(sceneUBO.LightPosition));
	glm::mat4 cubeTranslation = glm::mat4(1.0f);


	Sleet::Renderer::BeginRendering(m_Framebuffer);

	Sleet::Renderer::BindPipeline(m_SimplePipeline);
	Sleet::Renderer::BindDescriptorSet(m_TextureSet, 0);
	Sleet::Renderer::BindVertexBuffer(m_QuadVertex);
	Sleet::Renderer::BindIndexBuffer(m_QuadIndex);
	Sleet::Renderer::DrawIndexed(m_QuadIndex->GetIndexCount());

	Sleet::Renderer::EndRendering(m_Framebuffer);

	Sleet::Renderer::BeginSwapchainRendering();

	Sleet::Renderer::BindPipeline(m_SimplePipeline);
	Sleet::Renderer::BindDescriptorSet(m_FramebufferDescriptorSet[Sleet::Renderer::CurrentFrameIndex()], 0);
	//Sleet::Renderer::BindDescriptorSet(m_TextureSet, 0);
	Sleet::Renderer::BindVertexBuffer(m_QuadVertex);
	Sleet::Renderer::BindIndexBuffer(m_QuadIndex);
	Sleet::Renderer::DrawIndexed(m_QuadIndex->GetIndexCount());

	Sleet::Renderer::EndSwapchainRendering();

}

void ExampleLayer::OnImGuiRender()
{
	
}