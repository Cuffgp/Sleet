#pragma once

#include "Sleet/Core/Base.h"

#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"
#include "Sleet/Renderer/Texture.h"


namespace Sleet {

	class Mesh
	{
	public:
		Mesh();
		~Mesh();
	private:
		Ref<VertexBuffer> m_VertexBuffer = nullptr;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
	};

}
