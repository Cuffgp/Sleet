#pragma once

#include "tiny_gltf.h"

#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"

namespace Sleet {

	class GltfLoader 
	{
	public:
		GltfLoader(const std::string filepath);
		~GltfLoader();

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

	private:
		std::vector<uint8_t> GetAttributeData(const tinygltf::Model* model, uint32_t accessorId);

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}