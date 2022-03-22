#include "slpch.h"
#include "Sleet/Core/Application.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main()
{
	Sleet::Log::Init();
	Sleet::Application app{};

	try 
	{
		app.run();
	}

	catch(const std::exception &e)
	{
		SL_ERROR(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}