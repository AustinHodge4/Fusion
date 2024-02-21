project "glad"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
	    "include/glad/glad.h",
		"include/glad/KHR/khrplatform.h",
	    "src/glad.c"
	}

	includedirs
	{
		"include"
	}

	filter "system:linux"
		systemversion "latest"
		pic "on"

	filter "system:windows"
		systemversion "latest"
		-- buildoptions { "/MT" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
