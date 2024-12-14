workspace "Sleet"
	architecture "x64"
	--toolset "msc"
	toolset "clang"
	startproject "Sandbox"

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

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "Sleet/vendor/glfw/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["GLFW"] = "vendor/glfw"
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Cross_Reflect_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-reflectd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
Library["SPIRV_Cross_Reflect_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-reflect.lib"

group "Dependencies"
	include "Sleet/vendor/glfw"

project "Sleet"
	location "Sleet"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
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
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/tiny_gltf/",
		"%{prj.name}/vendor/glfw/include",
		"%{IncludeDir.VulkanSDK}"
	}
	
	links
	{
        "GLFW",
		"%{Library.Vulkan}",
	}
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "On"
		
		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_Reflect_Debug}"
		}

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"
		
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_Reflect_Release}"
		}

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Sleet/src",
		"%{wks.location}/Sleet/vendor",
		"%{wks.location}/Sleet/vendor/glfw/include",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"Sleet"
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