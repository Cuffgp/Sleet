#include "Sleet.h"
#include "Sleet/Core/EntryPoint.h"

//#include "ExampleLayer.h"
#include "SampleLayer.h"

class SandboxApp : public Sleet::Application
{
public:
	SandboxApp()
	{
		PushLayer(new SampleLayer());
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