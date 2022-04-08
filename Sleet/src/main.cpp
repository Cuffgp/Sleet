#include "slpch.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Core/Log.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main()
{
	Sleet::Log::Init();
	Sleet::Application app{};

	try 
	{
		app.Run();
	}

	catch(const std::exception &e)
	{
		SL_ERROR(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}