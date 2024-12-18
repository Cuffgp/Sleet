#pragma once

#include "glm/glm.hpp"

#include "tiny_gltf/tiny_gltf.h"

#include "Sleet/Core/Base.h"
#include "Sleet/Renderer/Texture.h"
#include "Sleet/Renderer/VertexBuffer.h"
#include "Sleet/Renderer/IndexBuffer.h"

namespace Sleet {

	class GltfLoader 
	{
	public:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 Uv;
		};

		// Single vertex and index buffer for all primitives
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		struct Node;

		// A primitive contains the data for a single draw call
		struct Primitive 
		{
			uint32_t FirstIndex;
			uint32_t IndexCount;
			int32_t MaterialIndex;
		};

		// Contains the node's (optional) geometry and can be made up of an arbitrary number of primitives
		struct Mesh 
		{
			std::vector<Primitive> Primitives;
		};

		// A node represents an object in the glTF scene graph
		struct Node 
		{
			Node* Parent;
			std::vector<Node*> Children;
			Mesh Mesh;
			glm::mat4 Matrix;
			~Node() { for (auto& child : Children) { delete child; } }
		};

		// A glTF material stores information in e.g. the texture that is attached to it and colors
		struct Material 
		{
			glm::vec4 baseColorFactor = glm::vec4(1.0f);
			uint32_t baseColorTextureIndex;
		};

		// Contains the texture for a single glTF image
		// Images may be reused by texture objects and are as such separated
		struct Image 
		{
			Ref<Sleet::Texture> Texture;
			// We also store (and create) a descriptor set that's used to access this texture from the fragment shader
			Ref<DescriptorSet> DescriptorSet;
		};

		// A glTF texture stores a reference to the image and a sampler
		// In this sample, we are only interested in the image
		struct Texture 
		{
			int32_t ImageIndex;
		};

		std::vector<Image> m_Images;
		std::vector<Texture> m_Textures;
		std::vector<Material> m_Materials;
		std::vector<Node*> m_Nodes;

		std::string m_ParentPath;

	public:
		GltfLoader(const std::string filepath);
		~GltfLoader();

		void DrawNode(Node* node);
		void Draw();
	private:
		void LoadImages(tinygltf::Model& input);
		void LoadTextures(tinygltf::Model& input);
		void LoadMaterials(tinygltf::Model& input);
		void LoadNode(
			const tinygltf::Node& inputNode,
			const tinygltf::Model& input,
			GltfLoader::Node* parent,
			std::vector<uint32_t>& indexBuffer,
			std::vector<GltfLoader::Vertex>& vertexBuffer);
			
	private:

	};

}