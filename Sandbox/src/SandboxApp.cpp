#include "Sleet.h"
#include "Sleet/Core/EntryPoint.h"

class SandboxApp : public Sleet::Application
{
public:
	SandboxApp()
	{

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