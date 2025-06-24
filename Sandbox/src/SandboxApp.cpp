#include "Sleet.h"
#include "Sleet/Core/EntryPoint.h"

#include "ExampleLayer.h"
#include "Render3DLayer.h"
#include "CitySimulationLayer.h"

class SandboxApp : public Sleet::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
	}

	~SandboxApp()
	{

	}
private:

};

Sleet::Application* Sleet::CreateApplication()
{
	return new SandboxApp();
}