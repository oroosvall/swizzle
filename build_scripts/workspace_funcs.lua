if _ACTION ~= nil then platform = _ACTION end

function defaultWorkspaceConfig()
    config = {}
    config["buildDir"] = "build/"
    config["disableTests"] = false
    config["disableApps"] = false

    return config
end

function loadWorkspaceConfig(configFile)
    defaultConfig = defaultWorkspaceConfig()

    config = json.decode(io.readfile(configFile))
    config = table.merge(defaultConfig, config)

    return config
end

function storeWorkspaceConfig(configFile, config)
    io.writefile(configFile, json.encode(config))
end

function setupWorkspace(buildDir)
workspace "Swizzle"
    architecture "x64"
    location(buildDir .. platform)
    debugdir("data/")
    configurations
    {
        "Debug",
        "RelWDbgInfo",
        "Release"
    }
    flags { "MultiProcessorCompile" }
    startproject "sandbox"

targetdir (buildDir .. platform .. "/bin-%{cfg.shortname}/")
objdir (buildDir .. platform .. "/int-%{cfg.shortname}/")

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
    flags { "FatalCompileWarnings", "NoBufferSecurityCheck" }
    omitframepointer "On"
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

    pic "On"

    links
    {
        "pthread",
        "X11",
        "xcb",
        "xcb-randr"
    }
    defines
    {
        "SW_LINUX"
    }
end