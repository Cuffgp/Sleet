#include "slpch.h"

#include "Sleet/Renderer/GltfLoader.h"
#include "Sleet/Renderer/Renderer.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf/tiny_gltf.h"

namespace Sleet {

	GltfLoader::GltfLoader(const std::string filepath)
	{
		SL_INFO("Loading gltf object");

		m_ParentPath = filepath.substr(0, filepath.find_last_of("/\\"));

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);

		if (!warn.empty()) 
		{
			SL_WARN("Warn: %s\n", warn.c_str());
		}

		if (!err.empty()) 
		{
			SL_ERROR("Err: %s\n", err.c_str());
		}

		if (!ret) 
		{
			SL_ERROR("Failed to parse glTF");
		}

		std::vector<uint32_t> indexBuffer;
		std::vector<Vertex> vertexBuffer;

		LoadImages(model);
		LoadTextures(model);
		LoadMaterials(model);

		const tinygltf::Scene& scene = model.scenes[0];
		for (size_t i = 0; i < scene.nodes.size(); i++) 
		{
			const tinygltf::Node node = model.nodes[scene.nodes[i]];
			LoadNode(node, model, nullptr, indexBuffer, vertexBuffer);
		}

		m_VertexBuffer = VertexBuffer::Create(vertexBuffer.data(), vertexBuffer.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(indexBuffer.data(), indexBuffer.size() * sizeof(uint32_t));

		SL_INFO("Done");

	}

	GltfLoader::~GltfLoader()
	{
		for (auto node : m_Nodes) 
		{
			delete node;
		}
	}

	void GltfLoader::LoadImages(tinygltf::Model& input)
	{
		DescriptorMap constantMap;
		constantMap[0] = ShaderDescriptor(1, 0, ShaderStage::Fragment, ShaderDescriptorType::ImageSampler, 1);

		m_Images.resize(input.images.size());
		for (size_t i = 0; i < input.images.size(); i++)
		{
			auto inputImage = input.images[i];
			SL_INFO("Loading Image {}", inputImage.name);
			
			m_Images[i].Texture = Sleet::Texture::Create(m_ParentPath + "/" + inputImage.uri);
			m_Images[i].DescriptorSet = DescriptorSet::Create(constantMap);
			m_Images[i].Texture->Write(m_Images[i].DescriptorSet, 0);

		}
	}

	void GltfLoader::LoadTextures(tinygltf::Model& input)
	{
		m_Textures.resize(input.textures.size());
		for (size_t i = 0; i < input.textures.size(); i++)
		{
			m_Textures[i].ImageIndex = input.textures[i].source;
		}
	}

	void GltfLoader::LoadMaterials(tinygltf::Model& input)
	{
		m_Materials.resize(input.materials.size());

		for (size_t i = 0; i < input.materials.size(); i++) 
		{
			// We only read the most basic properties required for our sample
			tinygltf::Material glTFMaterial = input.materials[i];
			// Get the base color factor
			if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) 
			{
				m_Materials[i].baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
			}
			// Get base color texture index
			if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) 
			{
				m_Materials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
			}
		}
	}

	void GltfLoader::LoadNode(
		const tinygltf::Node& inputNode,
		const tinygltf::Model& input,
		GltfLoader::Node* parent,
		std::vector<uint32_t>& indexBuffer,
		std::vector<GltfLoader::Vertex>& vertexBuffer)

	{
		Node* node = new Node{};
		node->Matrix = glm::mat4(1.0f);
		node->Parent = parent;

		// Get the local node matrix
		// It's either made up from translation, rotation, scale or a 4x4 matrix
		if (inputNode.translation.size() == 3) 
		{
			node->Matrix = glm::translate(node->Matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
		}
		if (inputNode.rotation.size() == 4) 
		{
			glm::quat q = glm::make_quat(inputNode.rotation.data());
			node->Matrix *= glm::mat4(q);
		}
		if (inputNode.scale.size() == 3) 
		{
			node->Matrix = glm::scale(node->Matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
		}
		if (inputNode.matrix.size() == 16) 
		{
			node->Matrix = glm::make_mat4x4(inputNode.matrix.data());
		};

		// Load node's children
		if (inputNode.children.size() > 0) 
		{
			for (size_t i = 0; i < inputNode.children.size(); i++) 
			{
				LoadNode(input.nodes[inputNode.children[i]], input, node, indexBuffer, vertexBuffer);
			}
		}

		// If the node contains mesh data, we load vertices and indices from the buffers
		// In glTF this is done via accessors and buffer views
		if (inputNode.mesh > -1)
		{
			const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
			// Iterate through all primitives of this node's mesh
			for (size_t i = 0; i < mesh.primitives.size(); i++)
			{
				const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
				uint32_t firstIndex = static_cast<uint32_t>(indexBuffer.size());
				uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
				uint32_t indexCount = 0;
				// Vertices
				{
					const float* positionBuffer = nullptr;
					const float* normalsBuffer = nullptr;
					const float* texCoordsBuffer = nullptr;
					size_t vertexCount = 0;

					// Get buffer data for vertex positions
					if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end())
					{
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
						const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
						positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
						vertexCount = accessor.count;
					}
					// Get buffer data for vertex normals
					if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end())
					{
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
						const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
						normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					}
					// Get buffer data for vertex texture coordinates
					// glTF supports multiple sets, we only load the first one
					if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end())
					{
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
						const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
						texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					}

					// Append data to model's vertex buffer
					for (size_t v = 0; v < vertexCount; v++)
					{
						Vertex vert{};
						vert.Position = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
						vert.Position.y *= -1;
						vert.Normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
						vert.Uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
						//vert.color = glm::vec3(1.0f);
						vertexBuffer.push_back(vert);
					}
				}

				// Indices
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
					const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

					indexCount += static_cast<uint32_t>(accessor.count);

					// glTF supports different component types of indices
					switch (accessor.componentType) 
					{
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: 
					{
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) 
						{
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: 
					{
						const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) 
						{
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: 
					{
						const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) 
						{
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						break;
					}
					default:
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
					}
				}
				Primitive primitive{};
				primitive.FirstIndex = firstIndex;
				primitive.IndexCount = indexCount;
				primitive.MaterialIndex = glTFPrimitive.material;
				node->Mesh.Primitives.push_back(primitive);
			}
		}
		if (parent) 
		{
			parent->Children.push_back(node);
		}
		else 
		{
			m_Nodes.push_back(node);
		}
	}

	void GltfLoader::DrawNode(Node* node)
	{
		if (node->Mesh.Primitives.size() > 0) 
		{
			// Pass the node's matrix via push constants
			// Traverse the node hierarchy to the top-most parent to get the final matrix of the current node
			glm::mat4 nodeMatrix = node->Matrix;
			Node* currentParent = node->Parent;
			while (currentParent) 
			{
				nodeMatrix = currentParent->Matrix * nodeMatrix;
				currentParent = currentParent->Parent;
			}
			// Pass the final matrix to the vertex shader using push constants
			Renderer::SetTransform(nodeMatrix);
			for (Primitive& primitive : node->Mesh.Primitives) 
			{
				if (primitive.IndexCount > 0) 
				{
					// Get the texture index for this primitive
					Texture texture = m_Textures[m_Materials[primitive.MaterialIndex].baseColorTextureIndex];
					// Bind the descriptor for the current primitive's texture to the first set
					Renderer::BindDescriptorSet(m_Images[texture.ImageIndex].DescriptorSet, 1);
					Renderer::DrawIndexed(primitive.IndexCount, primitive.FirstIndex);
				}
			}
		}
		for (auto& child : node->Children) 
		{
			DrawNode(child);
		}
	}

	void GltfLoader::Draw()
	{
		// All vertices and indices are stored in single buffers, so we only need to bind once
		Renderer::BindVertexBuffer(m_VertexBuffer);
		Renderer::BindIndexBuffer(m_IndexBuffer);

		// Render all nodes at top-level
		for (auto& node : m_Nodes) 
		{
			DrawNode(node);
		}
	}



}