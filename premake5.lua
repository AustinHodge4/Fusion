workspace "Fusion"
    architecture "x64"
    startproject "Sandbox"

    configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"] = "Fusion/vendor/glfw/include"
IncludeDir["glad"] = "Fusion/vendor/glad/include"
IncludeDir["imgui"] = "Fusion/vendor/imgui"
IncludeDir["glm"] = "Fusion/vendor/glm"

group "Dependencies"
    include "Fusion/vendor/glfw"
    include "Fusion/vendor/glad"
    include "Fusion/vendor/imgui"

group ""

project "Fusion"
    location "Fusion"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "fepch.h"
    pchsource "Fusion/src/fepch.cpp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}"
    }

    links
    {
        "glfw",
        "glad",
        "imgui",
        "opengl32.lib"
    }

    defines 
    {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

        defines 
        {
            "FE_PLATFORM_WINDOWS",
            "FE_BUILD_DLL"
        }
    
    filter "configurations:Debug"
        defines "FE_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "FE_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "FE_DIST"
        runtime "Release"
        optimize "on"

    
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Fusion/vendor/spdlog/include",
        "Fusion/src",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}"
    }

    links 
    {
        "Fusion"
    }

    filter "system:windows"
        systemversion "latest"

        defines 
        {
            "FE_PLATFORM_WINDOWS"
        }
    
    filter "configurations:Debug"
        defines "FE_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "FE_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "FE_DIST"
        runtime "Release"
        optimize "on"