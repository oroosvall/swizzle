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
        "OTP_PROFILING",
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
    flags { "FatalCompileWarnings", "NoBufferSecurityCheck" }

filter "configurations:Release"
    defines "SW_RELEASE"
    optimize "Speed"
    flags { "FatalCompileWarnings", "NoBufferSecurityCheck", "NoFramePointer" }
    inlining ("auto")

filter "system:windows"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    
    if (platform ~= "vs2019") then -- If on windows and not visual studio manually add some core defines and libs that are used
        defines
        {
            "_WIN32_WINNT=0x0A00", -- Windows 10
            "UNICODE"
        }
        links
        {
            "gdi32",
            "uuid",
            "ole32"
        }
    elseif (platform == "vs2019") then
        buildoptions { "/Ob2" }
    end

filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    links
    {
        "pthread",
        "X11"
    }
    defines
    {
        "SW_LINUX"
    }