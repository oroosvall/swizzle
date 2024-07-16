function generateVSCodeProjectSettings(projectTable, configFile)

    projectDefines = ""
    projectIncludes = [[
            "${workspaceFolder}/**",
            "${VULKAN_SDK}/include/**",
            "/usr/include/winpr2"
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
        "intelliSenseMode": "linux-gcc-x64",
        "compileCommands": ".vscode/compile_commands.json"
    }
    ],
    "version": 4
}]]

    io.writefile(configFile, vscodeProjectConfig)

end
