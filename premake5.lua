workspace "Sleet"
	architecture "x64"
	startproject "Sleet"

	configurations 
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Sleet/vendor/glfw/include"
--IncludeDir["ImGui"] = "Sleet/vendor/imgui"
IncludeDir["Vulkan"] = "C:/VulkanSDK/1.3.204.0/Include"
IncludeDir["glm"] = "Sleet/vendor/glm"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "C:/VulkanSDK/1.3.204.0/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"


group "Dependencies"
	include "Sleet/vendor/"

project "Sleet"
	location "Sleet"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "slpch.h"
	pchsource "Sleet/src/slpch.cpp"

	files 
	{ 
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		--"%{IncludeDir.ImGui}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.glm}",
		"%{prj.name}/vendor"
	}

	links
	{
		"GLFW",
		--"ImGui",
		"%{Library.Vulkan}"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "On"
		
		links
		{

		}

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"
		
		links
		{

		}