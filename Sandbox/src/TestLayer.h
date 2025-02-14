#pragma once

#include "Sleet.h"

using namespace Sleet;

class TestLayer : public Layer
{
public:
	TestLayer();
	virtual ~TestLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(float ts) override;
	virtual void OnImGuiRender() override;
private:

};