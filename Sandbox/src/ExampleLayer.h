#pragma once

#include "Sleet.h"


class ExampleLayer : public Sleet::Layer
{
public:
	struct SceneUBO
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec4 AmbientLight;
		glm::vec4 LightPosition;
		glm::vec4 LightColour;
	};
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(float ts) override;
	virtual void OnImGuiRender() override;
private:

	// Extra to be moved to sandbox
	Sleet::Ref<Sleet::Pipeline> m_SimplePipeline;
	Sleet::Ref<Sleet::VertexBuffer> m_CubeVertex;
	Sleet::Ref<Sleet::IndexBuffer> m_CubeIndex;
	Sleet::Ref<Sleet::Pipeline> m_MeshPipeline;
	Sleet::Ref<Sleet::VertexBuffer> m_VertexBuffer;
	Sleet::Ref<Sleet::IndexBuffer> m_IndexBuffer;
	Sleet::Ref<Sleet::UniformBuffer> m_UniformBuffer;
	Sleet::Ref<Sleet::DescriptorSet> m_ConstantSet;

	std::vector<Sleet::Ref<Sleet::DescriptorSet>> m_SceneDescriptorSet;
	std::vector<Sleet::Ref<Sleet::UniformBuffer>> m_SceneUniform;

	Sleet::Ref<Sleet::Texture> m_Texture;

	SceneUBO m_SceneUBO;
	Sleet::Camera m_Camera = Sleet::Camera(glm::vec3{ 0., 0., -40. }, glm::vec3{ 0., 0., 1. });
	float time;
};