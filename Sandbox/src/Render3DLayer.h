#pragma once

#include "Sleet.h"

using namespace Sleet;

class Render3DLayer : public Layer
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
	Render3DLayer();
	virtual ~Render3DLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(float ts) override;
	virtual void OnImGuiRender() override;
private:
	Ref<ObjLoader> m_CubeMesh;
	Ref<ObjLoader> m_SpotMesh;

	Ref<GltfLoader> m_Gltf;

	Ref<Texture> m_WoodTex;
	Ref<Texture> m_SpotTex;
	Ref<Pipeline> m_MeshPipeline;

	std::vector<Ref<DescriptorSet>> m_SceneSets;
	std::vector<Ref<UniformBuffer>> m_SceneUniforms;

	Ref<DescriptorSet> m_ModelSet1;
	Ref<DescriptorSet> m_ModelSet2;

	SceneUBO m_SceneUBO;
	Sleet::Camera m_Camera = Sleet::Camera(glm::vec3{ 0., 0., -40. }, glm::vec3{ 0., 0., 1. });

	float time = 0.0f;
};