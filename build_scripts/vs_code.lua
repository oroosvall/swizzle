function generateVSCodeProjectSettings(projectTable, configFile)

    projectDefines = ""
    projectIncludes = [[
            "${workspaceFolder}/**",
            "${VULKAN_SDK}/include/**"
]]

    vscodeProjectConfig =
    [[{
    "configurations": [
    {
        "name": "Linux",
        "includePath": [
]] .. projectIncludes .. [[
        ],
        "defines": [
]] .. projectDefines .. [[
        ],
        "compilerPath": "/usr/bin/gcc",
        "cStandard": "gnu17",
        "cppStandard": "gnu++17",
        "intelliSenseMode": "linux-gcc-x64"
    }
    ],
    "version": 4
}]]

    io.writefile(configFile, vscodeProjectConfig)

end
