#include <iostream>

#include "imgui/imgui.h"

#include "CitySimulationLayer.h"

#include "readosm.h"

// Callback function for nodes
int node_callback(const void* user_data, const readosm_node* node)
{
	auto& osmInfo = *static_cast<CitySimulationLayer::OSMInfo*>(const_cast<void*>(user_data));
	
	// 1. Store the coordinate
	osmInfo.NodeInfo.Coordinates[node->id] = { node->longitude, node->latitude };

	// 2. Update bounds (initialize on first node)
	if (osmInfo.NodeInfo.Coordinates.size() == 1)
	{
		// First node - initialize bounds
		osmInfo.NodeInfo.MinCoord = { node->longitude, node->latitude };
		osmInfo.NodeInfo.MaxCoord = { node->longitude, node->latitude };
	}
	else 
	{
		// Update min/max
		osmInfo.NodeInfo.MinCoord.first = std::min(osmInfo.NodeInfo.MinCoord.first, node->longitude);
		osmInfo.NodeInfo.MinCoord.second = std::min(osmInfo.NodeInfo.MinCoord.second, node->latitude);
		osmInfo.NodeInfo.MaxCoord.first = std::max(osmInfo.NodeInfo.MaxCoord.first, node->longitude);
		osmInfo.NodeInfo.MaxCoord.second = std::max(osmInfo.NodeInfo.MaxCoord.second, node->latitude);
	}

	return READOSM_OK;
}

// Empty callbacks for ways and relations
int way_callback(const void* user_data, const readosm_way* way)
{
	auto& osmInfo = *static_cast<CitySimulationLayer::OSMInfo*>(const_cast<void*>(user_data));

	// 1. Check if this is a residential building
	bool is_house = false;
	CitySimulationLayer::Building house;
	house.Id = way->id;

	for (int i = 0; i < way->tag_count; i++) 
	{
		const auto& tag = way->tags[i];

		// Building type check
		if (strcmp(tag.key, "building") == 0) 
		{
			is_house = true;
		}
		// Address extraction
		else if (strcmp(tag.key, "addr:street") == 0) 
		{
			house.Street = tag.value;
		}
		else if (strcmp(tag.key, "addr:housenumber") == 0) 
		{
			house.Housenumber = tag.value;
		}
		else if (strcmp(tag.key, "addr:postcode") == 0) 
		{
			house.Postcode = tag.value;
		}
	}

	if (!is_house) return READOSM_OK;

	// 2. Validate geometry (closed polygon)
	if (way->node_ref_count < 4 ||
		way->node_refs[0] != way->node_refs[way->node_ref_count - 1]) 
	{
		SL_WARN("House {} has invalid polygon", way->id);
		return READOSM_OK;
	}

	// 3. Build polygon from node references
	house.Polygon.reserve(way->node_ref_count);
	for (int i = 0; i < way->node_ref_count; i++) 
	{
		auto it = osmInfo.NodeInfo.Coordinates.find(way->node_refs[i]);
		if (it != osmInfo.NodeInfo.Coordinates.end())
		{
			house.Polygon.push_back(it->second);
		}
		else 
		{
			std::cerr << "Missing node " << way->node_refs[i]
				<< " for house " << way->id << "\n";
		}
	}

	// 4. Store if valid
	if (house.Polygon.size() == way->node_ref_count) 
	{
		osmInfo.Building.push_back(std::move(house));
	}
	else {
		std::cerr << "House " << way->id << " has incomplete geometry\n";
	}

	return READOSM_OK;
}

int relation_callback(const void* user_data, const readosm_relation*)
{
	return READOSM_OK;
}

CitySimulationLayer::CitySimulationLayer()
{
	m_Pipeline = Sleet::Pipeline::Create("assets/shaders/map_shader");

	const void* osm_handle;
	int ret;

	ret = readosm_open("assets/data/map.osm", &osm_handle);
	if (ret != READOSM_OK)
	{
		SL_ERROR("Failed to read OSM file");
	}

	ret = readosm_parse(osm_handle,&m_OSMInfo, node_callback, nullptr, nullptr);
	if (ret != READOSM_OK)
	{
		SL_ERROR("Failed to parse OSM nodes");
	}
	readosm_close(osm_handle);


	ret = readosm_open("assets/data/map.osm", &osm_handle);
	if (ret != READOSM_OK)
	{
		SL_ERROR("Failed to read OSM file");
	}
	ret = readosm_parse(osm_handle, &m_OSMInfo, nullptr, way_callback, nullptr);

	if (ret != READOSM_OK)
	{
		SL_ERROR("Failed to parse OSM ways");
	}
	readosm_close(osm_handle);

	TriangulatePolygon();
}

void CitySimulationLayer::OnAttach()
{

}

void CitySimulationLayer::OnDetach()
{

}

void CitySimulationLayer::OnUpdate(float ts)
{
	m_Camera.OnUpdate(ts);

	Sleet::Renderer::BindPipeline(m_Pipeline);
	Sleet::Renderer::SetTransform(m_Camera.GetProjectionView());

	uint32_t nBuildings = m_VertexBuffers.size();

	for (uint32_t i = 0; i < nBuildings; i++)
	{
		Sleet::Renderer::BindVertexBuffer(m_VertexBuffers[i]);
		Sleet::Renderer::BindIndexBuffer(m_IndexBuffers[i]);
		Sleet::Renderer::DrawIndexed(m_IndexBuffers[i]->GetIndexCount());
	}

}

void CitySimulationLayer::OnImGuiRender()
{

}

void CitySimulationLayer::TriangulatePolygon()
{
	auto minCoord = m_OSMInfo.NodeInfo.MinCoord;
	auto maxCoord = m_OSMInfo.NodeInfo.MaxCoord;

	SL_INFO("OSM bounds - Min: {},{} Max: {},{}",
		minCoord.first, minCoord.second,
		maxCoord.first, maxCoord.second);

	// Initialize with first vertex of first building
	auto firstVertex = m_OSMInfo.Building[0].Polygon[0];
	std::pair<double, double> m_HouseMinCoord = { firstVertex.first, firstVertex.second };
	std::pair<double,double> m_HouseMaxCoord = { firstVertex.first, firstVertex.second };

	// Iterate through all houses and their vertices
	for (const auto& building : m_OSMInfo.Building)
	{
		for (const auto& vertex : building.Polygon)
		{
			// Update min/max longitude (X)
			m_HouseMinCoord.first = std::min(m_HouseMinCoord.first, vertex.first);
			m_HouseMaxCoord.first = std::max(m_HouseMaxCoord.first, vertex.first);

			// Update min/max latitude (Y)
			m_HouseMinCoord.second = std::min(m_HouseMinCoord.second, vertex.second);
			m_HouseMaxCoord.second = std::max(m_HouseMaxCoord.second, vertex.second);
		}
	}

	SL_INFO("OSM house only bounds - Min: {},{} Max: {},{}",
		m_HouseMinCoord.first, m_HouseMinCoord.second,
		m_HouseMaxCoord.first, m_HouseMaxCoord.second);

	minCoord = m_HouseMinCoord;
	maxCoord = m_HouseMaxCoord;

	const double width = maxCoord.first - minCoord.first;
	const double height = maxCoord.second - minCoord.second;
	const double centerLon = (minCoord.first + maxCoord.first) / 2.0;
	const double centerLat = (minCoord.second + maxCoord.second) / 2.0;
	const double maxDimension = std::max(width, height);
	const double scale = 2.0 / maxDimension;


	for (const auto& building : m_OSMInfo.Building)
	{

		std::vector<glm::vec2> vertices;
		std::vector<uint32_t> indices;

		// 1. Transform and store vertices
		for (const auto& vertex : building.Polygon)
		{
			// Normalize coordinates to [-1, 1] range (Vulkan NDC)
			glm::vec2 coord(
				(vertex.first - centerLon) * scale,
				(vertex.second - centerLat) * scale
			);
			vertices.push_back(coord);
		}

		// Remove duplicate closing vertex if present
		if (vertices.size() > 1 && vertices.front() == vertices.back())
		{
			vertices.pop_back();
		}

		// 2. Simple ear-clipping triangulation for convex polygons
		if (vertices.size() >= 3)
		{
			for (uint32_t i = 1; i < vertices.size() - 1; ++i)
			{
				indices.push_back(0);
				indices.push_back(i);
				indices.push_back(i + 1);
			}
		}

		m_VertexBuffers.push_back(Sleet::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(glm::vec2)));
		m_IndexBuffers.push_back(Sleet::IndexBuffer::Create(indices.data(), indices.size() * sizeof(uint32_t)));
	}
}