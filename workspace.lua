if _ACTION ~= nil then platform = _ACTION end

workspace "Swizzle"
	architecture "x64"
	location("build/" .. platform)
	debugdir("data/")
	configurations
	{
		"Debug",
		"RelWDbgInfo",
		"Release"
	}
	flags { "MultiProcessorCompile" }
	startproject "sandbox"

targetdir ("build/" .. platform .. "/bin-%{cfg.shortname}/")
objdir ("build/" .. platform .. "/int-%{cfg.shortname}/")

warnings "Extra"

filter "configurations:Debug"
	defines
	{
		"SW_DEBUG",
		"OTP_PROFILING"
	}

	symbols "On"

filter "configurations:RelWDbgInfo"
	defines
	{
		"SW_REL_W_DBG",
		"OTP_PROFILING"
	}
	symbols "On"
	optimize "On"
	flags { "FatalCompileWarnings" }

filter "configurations:Release"
	defines "SW_RELEASE"
	optimize "Speed"
    flags { "FatalCompileWarnings" }
    
filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
