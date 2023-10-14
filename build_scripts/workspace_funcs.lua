if _ACTION ~= nil then platform = _ACTION end

function default_workspace_config()
    config = {}
    config["buildDir"] = "build/"
    config["disableTests"] = false
    config["disableApps"] = false

    return config
end

function load_workspace_config(configFile)
    defaultConfig = default_workspace_config()

    config = json.decode(io.readfile(configFile))
    config = table.merge(defaultConfig, config)

    return config
end

function store_workspace_config(configFile, config)
    io.writefile(configFile, json.encode(config))
end

function setup_workspace(ws_cfg)
workspace(ws_cfg.name)
    buildDir = _WORKING_DIR .. "/" .. ws_cfg.build_dir
    architecture "x64"
    location(buildDir .. platform)
    debugdir("data/")
    configurations
    {
        "Debug",
        "RelWDbgInfo",
        "Release",
        "Asan"
    }
    flags { "MultiProcessorCompile" }
    startproject(ws_cfg.startup_project)

targetdir (buildDir .. platform .. "/bin-%{cfg.shortname}/")
objdir (buildDir .. platform .. "/int-%{cfg.shortname}/")

warnings "Extra"

filter {"configurations:Asan", "system:windows", "action:vs2022"}
    defines
    {
        "SW_DEBUG",
        "OTP_PROFILING",
    }
    symbols "On"
    sanitize "address"
    -- buildoptions { "/fsanitize=address", "/Zi" }
    editandcontinue "Off"
    flags { "NoIncrementalLink" }
    ignoredefaultlibraries { "LIBCMT" }

filter {"configurations:Asan", "system:linux"}
    defines
    {
        "SW_DEBUG",
        "OTP_PROFILING",
    }
    symbols "On"
    buildoptions { "-fsanitize=address", "-static-libasan" }

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
    staticruntime "Off"
    systemversion "latest"
    
    if (platform == "vs2019" or platform == "vs2022") then
        buildoptions { "/Ob2" }
    elseif (platform ~= "vs2019") then -- If on windows and not visual studio manually add some core defines and libs that are used
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