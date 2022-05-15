
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

function addDependencies(project, dependencies)
    project.dep = table.join(project.dep, dependencies)
end

function addExternalHeaders(project, headerPath)
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

function generateProject(projectTable, prjName)
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
    location("build/" .. platform .."/%{prj.name}")
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
    for i,vp in pairs(prj.vpaths) do
        vpaths({[vp.name] = vp.value})
    end

    links(projectLinks)
end