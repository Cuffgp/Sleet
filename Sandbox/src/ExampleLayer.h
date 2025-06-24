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

	Sleet::Ref<Sleet::Framebuffer> m_Framebuffer;

	Sleet::Ref<Sleet::VertexBuffer> m_QuadVertex;
	Sleet::Ref<Sleet::IndexBuffer> m_QuadIndex;
	Sleet::Ref<Sleet::VertexBuffer> m_CubeVertex;
	Sleet::Ref<Sleet::IndexBuffer> m_CubeIndex;

	std::vector<Sleet::Ref<Sleet::DescriptorSet>> m_SceneDescriptorSet;
	std::vector<Sleet::Ref<Sleet::DescriptorSet>> m_FramebufferDescriptorSet;
	std::vector<Sleet::Ref<Sleet::UniformBuffer>> m_SceneUniform;

	Sleet::Ref<Sleet::Texture> m_Texture;
	Sleet::Ref<Sleet::DescriptorSet> m_TextureSet;

	SceneUBO m_SceneUBO;
	Sleet::Camera m_Camera = Sleet::Camera(glm::vec3{ 0., 0., -40. }, glm::vec3{ 0., 0., 1. });
	float time;
};