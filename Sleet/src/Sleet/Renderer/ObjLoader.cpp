#include "slpch.h"

#include "Sleet/Renderer/ObjLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader/tiny_obj_loader.h"

namespace Sleet {

	ObjLoader::ObjLoader(const std::string filepath)
	{
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) 
        {
            SL_ERROR("TinyOBJ: {}",(warn + err));
        }

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (const auto& shape : shapes) 
        {
            for (const auto& index : shape.mesh.indices) 
            {
                Vertex vertex{};

                vertex.Position.x =  attrib.vertices[3 * index.vertex_index + 0];
                vertex.Position.y = -attrib.vertices[3 * index.vertex_index + 1];
                vertex.Position.z =  attrib.vertices[3 * index.vertex_index + 2];

                if (attrib.normals.size() > 0)
                {
                    vertex.Normal.x = attrib.normals[3 * index.normal_index + 0];
                    vertex.Normal.y = attrib.normals[3 * index.normal_index + 1];
                    vertex.Normal.z = attrib.normals[3 * index.normal_index + 2];
                }

                vertex.Uv.x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.Uv.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }

        m_VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex));
        m_IndexBuffer = IndexBuffer::Create(indices.data(), indices.size() * sizeof(uint32_t));
	}

	ObjLoader::~ObjLoader()
	{

	}

}