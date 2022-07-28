
function generateDefaultProjectConfig()

    defaultConfig = {}
    defaultConfig['kind'] = ""
    defaultConfig['language'] = ""
    defaultConfig['files'] = {}
    defaultConfig['pub_inc_dirs'] = {}
    defaultConfig['includedirs'] = {}
    defaultConfig['extheaders'] = {}
    defaultConfig['extlibdir'] = {}
    defaultConfig['extlibs'] = {}
    defaultConfig['dep'] = {}
    defaultConfig['defines'] = {}
    defaultConfig['vpaths'] = {}
    defaultConfig['postbuildcommands'] = {}

    return defaultConfig
end

function loadProjectConfig(configFile)
    print("Loading \"" .. configFile .."\"")

    defaultConfig = generateDefaultProjectConfig()

    configFilePath = path.getdirectory(configFile) .. "/"
    config = json.decode(io.readfile(configFile))

    config = table.merge(defaultConfig, config)

    for i, f in pairs(config.files) do
        config.files[i] = configFilePath .. f
    end
    for i, f in pairs(config.includedirs) do
        config.includedirs[i] = configFilePath .. f
    end
    for i, f in pairs(config.pub_inc_dirs) do
        config.pub_inc_dirs[i] = configFilePath .. f
    end
    for i, vpath in pairs(config.vpaths) do
        vpath.value = configFilePath .. vpath.value
        config.vpaths[i] = vpath
    end

    return config
end

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

function addDependencies(project, dependencies)
    project.dep = table.join(project.dep, dependencies)
end

function addExternalHeaders(project, headerPath)
    project.extheaders = table.join(project.extheaders, headerPath)
end

function addExternalHeadersProjectList(project, projects, headerPath)
    tbl = table.join(projects)
    for i, prj in pairs(tbl) do
        addExternalHeaders(project[prj], headerPath)
    end
    project.extheaders = table.join(project.extheaders, headerPath)
end

function addExternalLibDir(project, libDir)
    project.extlibdir = table.join(project.extlibdir, libDir)
end

function addExternalLib(project, libName)
    project.extlibs = table.join(project.extlibs, libName)
end

function addDefine(project, define)
    project.defines = table.join(project.defines, define)
end

function addPostBuildCommands(project, commands)
    project.postbuildcommands = table.join(project.postbuildcommands, commands)
end

function generateProject(projectTable, buildDir, prjName)
    projectName = prjName
    prj = projectTable[prjName]
    projectFiles = prj.files
    projectIncludeDirs = table.join(prj.pub_inc_dirs, prj.includedirs, prj.extheaders)
    projectLinks = {}
    projectDefs = prj.defines
    projectLibDirs = prj.extlibdir
    for i, dep in pairs(prj.dep) do
        link = projectTable[dep]
        table.insert(projectLinks, dep)
        projectIncludeDirs = table.join(projectIncludeDirs, link.pub_inc_dirs)
    end

    for i, lnk in pairs(prj.extlibs) do
        table.insert(projectLinks, lnk)
    end

    project(prjName)
    location(buildDir .. platform .."/%{prj.name}")
    objdir ("!" .. buildDir .. platform .. "/int-%{cfg.shortname}/%{prj.name}/")
    kind(prj.kind)
    language(prj.language)
    files(projectFiles)
    includedirs(projectIncludeDirs)
    libdirs(projectLibDirs)
    if (projectDefs ~= nil and (projectDefs ~= "" or type(projectDefs) == "table")) then
        for i,j in pairs(projectDefs) do
            if(j ~= "") then
                defines(j)
            end
        end
    end
    for i,j in pairs(prj.postbuildcommands) do
        postbuildcommands(j)
    end
    for i,vp in pairs(prj.vpaths) do
        vpaths({[vp.name] = vp.value})
    end
    
    links(projectLinks)
end

function generateTestProject(projectTable, buildDir, prjName, reportLinks)
    generateProject(projectTable, buildDir, prjName)
    filter {"system:linux", "toolset:gcc"}
        buildoptions {"-fprofile-arcs -ftest-coverage "}
        linkoptions {"-lgcov --coverage"}
        postbuildcommands("{MKDIR} %{wks.location}/reports/")
        postbuildcommands("./%{cfg.linktarget.relpath}")
        l = ""
        if (reportLinks ~= nil) then
            for i,j in pairs(table.join(reportLinks)) do
                l = l .. "%{cfg.objdir}/../" .. j .. " "
            end
        end
        postbuildcommands("gcovr -r . --html-details %{wks.location}/reports/%{prj.name}.html --object-directory %{cfg.objdir} %{cfg.objdir} " .. l .." -f '(.+)?[ch]pp$$'")
end