#pragma once

#include "Sleet/Core/Base.h"

#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"
#include "Sleet/Renderer/Texture.h"
#include "Sleet/Renderer/DescriptorSet.h"

namespace Sleet {

	class Mesh
	{
	public:
		Mesh();
		~Mesh();


	private:
		Ref<VertexBuffer> m_VertexBuffer = nullptr;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		Ref<Texture> m_Texture = nullptr;
		glm::mat4 transform = glm::mat4(1.0f);
		Ref<DescriptorSet> m_Set;
	};

}
