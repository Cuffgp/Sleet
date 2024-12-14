#pragma once

#include "Sleet/Renderer/Texture.h"
#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"

namespace Sleet {

	class ObjLoader
	{
	public:
		struct Vertex
		{
			glm::vec3 Position = { 0., 0., 0. };
			glm::vec3 Normal = { 0., 0., 0. };
			glm::vec2 Uv = { 0., 0.};
		};

		struct Material
		{

		};
	public:
		ObjLoader(const std::string filepath, const std::string matpath);
		ObjLoader(const std::string filepath);
		~ObjLoader();

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		Ref<Texture> GetTexture() { return m_Texture; }
	private:
		Ref<Texture> m_Texture;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};


}