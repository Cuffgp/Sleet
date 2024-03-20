#include "slpch.h"

#include "Sleet/Renderer/GltfLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

namespace Sleet {

	GltfLoader::GltfLoader(const std::string filepath)
	{

		SL_INFO("Loading gltf object");

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

		SL_INFO("Done");

        auto indexData = GetAttributeData(&model, model.meshes[0].primitives[0].indices);
        auto vertexPosition = GetAttributeData(&model, model.meshes[0].primitives[0].attributes["POSITION"]);
        auto vertexNormal = GetAttributeData(&model, model.meshes[0].primitives[0].attributes["NORMAL"]);
        auto vertexTexture = GetAttributeData(&model, model.meshes[0].primitives[0].attributes["TEXCOORD_0"]);
        auto vertexTangent = GetAttributeData(&model, model.meshes[0].primitives[0].attributes["TANGENT"]);

		SL_INFO("Index Size {}", indexData.size() / sizeof(uint16_t));
		SL_INFO("Position Size {}", vertexPosition.size() / (sizeof(glm::vec3)));
		SL_INFO("Texture Size {}", vertexTexture.size() / (sizeof(glm::vec2)));
		SL_INFO("Normal Size {}", vertexNormal.size() / (sizeof(glm::vec3)));
		SL_INFO("Tangent Size {}", vertexTangent.size() / (sizeof(glm::vec4)));

		uint32_t nIndices = indexData.size() / sizeof(uint16_t);
		uint32_t nVertices = vertexPosition.size() / sizeof(glm::vec3);

		std::vector<uint32_t> indexBuffer;
		std::vector<float> vertexBuffer;

		const uint16_t* indexU16 = reinterpret_cast<const uint16_t*>(indexData.data());

		for (int i = 0; i < nIndices; i++)
		{
			indexBuffer.push_back(uint32_t(indexU16[i]));
		}

		m_IndexBuffer = IndexBuffer::Create(indexBuffer.data(), nIndices * sizeof(uint32_t));

		for (int i = 0; i < nVertices; i++)
		{
			const glm::vec3* positionVec3 = reinterpret_cast<const glm::vec3*>(vertexPosition.data());
			const glm::vec3* normalVec3 = reinterpret_cast<const glm::vec3*>(vertexNormal.data());
			const glm::vec2* textureVec2 = reinterpret_cast<const glm::vec2*>(vertexTexture.data());

			vertexBuffer.push_back(positionVec3[i].x);
			vertexBuffer.push_back(positionVec3[i].y);
			vertexBuffer.push_back(positionVec3[i].z);

			vertexBuffer.push_back(normalVec3[i].x);
			vertexBuffer.push_back(normalVec3[i].y);
			vertexBuffer.push_back(normalVec3[i].z);

			vertexBuffer.push_back(textureVec2[i].x);
			vertexBuffer.push_back(textureVec2[i].y);
		}

		m_VertexBuffer = VertexBuffer::Create(vertexBuffer.data(), nVertices * 8 * sizeof(float));

	}

	GltfLoader::~GltfLoader()
	{

	}

    std::vector<uint8_t> GltfLoader::GetAttributeData(const tinygltf::Model* model, uint32_t accessorId)
    {
        SL_ASSERT((accessorId < model->accessors.size()), "Accessor not found");
        auto& accessor = model->accessors[accessorId];

        SL_ASSERT((accessor.bufferView < model->bufferViews.size()), "Buffer views not found");
        auto& bufferView = model->bufferViews[accessor.bufferView];

        SL_ASSERT((bufferView.buffer < model->buffers.size()), "Buffer not found");
        auto& buffer = model->buffers[bufferView.buffer];

        size_t stride = accessor.ByteStride(bufferView);
        size_t startByte = accessor.byteOffset + bufferView.byteOffset;
        size_t endByte = startByte + accessor.count * stride;
		
		SL_INFO("Stride {}, Start {}, End {}", stride, startByte, endByte);

        return std::vector<uint8_t>(buffer.data.begin() + startByte, buffer.data.begin() + endByte);
    }

}