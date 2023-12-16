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

project "Fusion"
    location "Fusion"
    kind "SharedLib"
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
        "%{prj.name}/vendor/spdlog/include"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines 
        {
            "FE_PLATFORM_WINDOWS",
            "FE_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }
    
    filter "configurations:Debug"
        defines "FE_DEBUG"
        symbols "On"
    
    filter "configurations:Release"
        defines "FE_RELEASE"
        symbols "On"
    
    filter "configurations:Dist"
        defines "FE_DIST"
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
        symbols "On"
    
    filter "configurations:Release"
        defines "FE_RELEASE"
        symbols "On"
    
    filter "configurations:Dist"
        defines "FE_DIST"
        symbols "On"