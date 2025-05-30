#include "Sleet.h"
#include "Sleet/Core/EntryPoint.h"

#include "ExampleLayer.h"
#include "Render3DLayer.h"

class SandboxApp : public Sleet::Application
{
public:
	SandboxApp()
	{
		PushLayer(new Render3DLayer());
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