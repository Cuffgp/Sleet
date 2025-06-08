#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "Sleet.h"

using namespace Sleet;

class CitySimulationLayer : public Layer
{
public:

	struct NodeInfo
	{
		std::unordered_map<int64_t, std::pair<double, double>> Coordinates;
		std::pair<double, double> MinCoord;
		std::pair<double, double> MaxCoord;
	};

	struct Building
	{
		int64_t Id;
		std::string Street;
		std::string Housenumber;
		std::string Postcode;
		std::vector<std::pair<double, double>> Polygon; // lon,lat vertices
	};

	struct OSMInfo
	{
		NodeInfo NodeInfo;
		std::vector<Building> Building;
	};

public:
	CitySimulationLayer();
	virtual ~CitySimulationLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(float ts) override;
	virtual void OnImGuiRender() override;

private:
	void TriangulatePolygon();

private:
	Sleet::OrthographicCamera m_Camera;
	Sleet::Ref<Pipeline> m_Pipeline;

	OSMInfo m_OSMInfo;
	std::vector<Sleet::Ref<VertexBuffer>> m_VertexBuffers;
	std::vector<Sleet::Ref<IndexBuffer>> m_IndexBuffers;

};