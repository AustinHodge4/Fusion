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

include "Fusion/vendor/glfw"
include "Fusion/vendor/glad"

project "Fusion"
    location "Fusion"
    kind "SharedLib"
    language "C++"

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
        "%{IncludeDir.glad}"
    }

    links
    {
        "glfw",
        "glad",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines 
        {
            "FE_PLATFORM_WINDOWS",
            "FE_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }
    
    filter "configurations:Debug"
        defines "FE_DEBUG"
        buildoptions "/MDd"
        symbols "On"
    
    filter "configurations:Release"
        defines "FE_RELEASE"
        buildoptions "/MD"
        symbols "On"
    
    filter "configurations:Dist"
        defines "FE_DIST"
        buildoptions "/MD"
        symbols "On"

    
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

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
        "Fusion/src"
    }

    links 
    {
        "Fusion"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines 
        {
            "FE_PLATFORM_WINDOWS"
        }
    
    filter "configurations:Debug"
        defines "FE_DEBUG"
        buildoptions "/MDd"
        symbols "On"
    
    filter "configurations:Release"
        defines "FE_RELEASE"
        buildoptions "/MD"
        symbols "On"
    
    filter "configurations:Dist"
        defines "FE_DIST"
        buildoptions "/MD"
        symbols "On"