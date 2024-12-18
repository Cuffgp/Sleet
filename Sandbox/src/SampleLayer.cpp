#include "SampleLayer.h"

SampleLayer::SampleLayer()
{
	m_CubeMesh = CreateRef<ObjLoader>("assets/mesh/cube.obj");
	m_SpotMesh = CreateRef<ObjLoader>("assets/mesh/spot_control_mesh.obj");

	//m_Gltf = CreateRef<GltfLoader>("assets/gltf/Cube/Cube.gltf");
	//m_Gltf = CreateRef<GltfLoader>("assets/gltf/AntiqueCamera/AntiqueCamera.gltf");
	m_Gltf = CreateRef<GltfLoader>("assets/gltf/sponza/sponza.gltf");

	m_WoodTex = Texture::Create("assets/texture/WoodFloor.png");
	m_SpotTex = Texture::Create("assets/texture/spot_texture.png");

	DescriptorMap sceneMap;
	sceneMap[0] = ShaderDescriptor(0, 0, ShaderStage::All, ShaderDescriptorType::Uniform, 1);

	for (int i = 0; i < Renderer::FramesInFlight(); i++)
	{
		m_SceneSets.push_back(DescriptorSet::Create(sceneMap));
		m_SceneUniforms.push_back(UniformBuffer::Create(sizeof(SceneUBO)));

		m_SceneUniforms[i]->Write(m_SceneSets[i], 0);
	}

	DescriptorMap constantMap;
	constantMap[0] = ShaderDescriptor(1, 0, ShaderStage::Fragment, ShaderDescriptorType::ImageSampler, 1);

	m_ModelSet1 = DescriptorSet::Create(constantMap);
	m_WoodTex->Write(m_ModelSet1, 0);

	m_ModelSet2 = DescriptorSet::Create(constantMap);
	m_SpotTex->Write(m_ModelSet2, 0);

	m_MeshPipeline = Pipeline::Create("assets/shaders/mesh_shader");

	m_Camera = Camera(glm::vec3{ 0., 0., -4. }, glm::vec3{ 0., 0., 1. });

	//SL_INFO("Index Count {}", m_IndexBuffer->GetIndexCount());
	time = 0;
}


void SampleLayer::OnAttach()
{

}

void SampleLayer::OnDetach()
{

}

void SampleLayer::OnUpdate(float ts)
{
	time += ts;
	m_Camera.OnUpdate(ts);

	m_SceneUBO.View = m_Camera.GetView();
	m_SceneUBO.Projection = m_Camera.GetProjection();

	//float intensity = std::sin(time)*std::sin(time);
	float intensity = 0.5f;

	m_SceneUBO.AmbientLight = glm::vec4(intensity, intensity, intensity, 1.0);
	m_SceneUBO.LightPosition = glm::vec4(3 * std::cos(time), 3 * std::sin(time), 0., 0.);
	m_SceneUBO.LightColour = glm::vec4(0.2, 0.6, 1., 0.);

	m_SceneUniforms[Renderer::CurrentFrameIndex()]->SetData(&m_SceneUBO);


	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 1, 0));
	glm::mat4 spotTranslation = rotation*glm::mat4(1.0f);

	glm::mat4 cubeScale = glm::scale(glm::mat4(1.0), glm::vec3(0.2, 0.2, 0.2));
	glm::mat4 cubeTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(m_SceneUBO.LightPosition));

	Renderer::BindPipeline(m_MeshPipeline);
	Renderer::BindDescriptorSet(m_SceneSets[Renderer::CurrentFrameIndex()], 0);

	m_Gltf->Draw();
	// Cube 1
	
	Renderer::BindVertexBuffer(m_CubeMesh->GetVertexBuffer());
	Renderer::BindIndexBuffer(m_CubeMesh->GetIndexBuffer());
	Renderer::BindDescriptorSet(m_ModelSet1, 1);
	Renderer::SetTransform(cubeScale*cubeTranslation);
	Renderer::DrawIndexed(m_CubeMesh->GetIndexBuffer()->GetIndexCount());

	/*
	Renderer::BindVertexBuffer(m_SpotMesh->GetVertexBuffer());
	Renderer::BindIndexBuffer(m_SpotMesh->GetIndexBuffer());
	Renderer::BindDescriptorSet(m_ModelSet2, 1);
	Renderer::SetTransform(spotTranslation);
	Renderer::DrawIndexed(m_SpotMesh->GetIndexBuffer()->GetIndexCount());
	*/

}

void SampleLayer::OnImGuiRender()
{

}