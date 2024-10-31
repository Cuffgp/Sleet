#include "slpch.h"

#include "Sleet/Renderer/ObjLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader/tiny_obj_loader.h"

namespace Sleet {

	ObjLoader::ObjLoader(const std::string filepath, const std::string matpath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str(), matpath.c_str()))
		{
			SL_ERROR("Failed to load obj");
		}

		SL_ERROR(warn + err);

		SL_INFO("Object Loaded");

		for (const auto& shape : shapes)
		{
			SL_INFO("Shape: {}", shape.name.c_str());

			for (const auto& mesh : shape.mesh.indices)
			{
			
			}
		}
	}

	ObjLoader::ObjLoader(const std::string filepath)
	{
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = Utils::GetParentDirectory(filepath);

		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filepath, reader_config)) 
		{
			if (!reader.Error().empty()) 
			{
				SL_ERROR("TinyObjReader: {}", reader.Error());
			}
			SL_ASSERT(false, "Error Loading object");
		}

		if (!reader.Warning().empty()) 
		{
			SL_WARN("TinyObjReader: {}", reader.Warning());
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		SL_INFO("Loaded model");

		// Loop over shapes
		for (size_t s = 0; s < 1; s++) 
		{
			std::vector<Vertex> m_Vertices;

			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
			{
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) 
				{
					Vertex vertex;

					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					vertex.Position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					vertex.Position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					vertex.Position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					// Check if `normal_index` is zero or positive. negative = no normal data
					if (idx.normal_index >= 0) 
					{
						vertex.Normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
						vertex.Normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
						vertex.Normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					if (idx.texcoord_index >= 0) 
					{
						vertex.Uv.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						vertex.Uv.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					}

					// Optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

					m_Vertices.push_back(vertex);
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}
	}

	ObjLoader::~ObjLoader()
	{

	}

}