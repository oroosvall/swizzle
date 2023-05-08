ProjectConfig = {
    name = "",
    kind = "",
    group = "",
    language = "",
    sources = {},
    inc_dirs = {},
    pub_inc_dirs = {},
    external_headers = {},
    external_lib_dir = {},
    external_libs = {},
    dependencies = {},
    defines = {},
    enabled_defines = {},
    post_build_commands = {},
    build_flags = {}
}

function ProjectConfig:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    return o
end

function ProjectConfig:load_from_json(file)
    configFilePath = path.getdirectory(file) .. "/"
    config = json.decode(io.readfile(file))

    if config.sources == nil then
        print("missing sources")
        os.exit(-1)
    end

    self.kind = config.kind
    self.language = config.language
    self.sources = config.sources
    self.inc_dirs = config.inc_dirs
    self.pub_inc_dirs = config.pub_inc_dirs
    self.external_headers = config.external_headers
    self.external_lib_dir = config.external_lib_dir
    self.external_libs = config.external_libs
    self.dependencies = config.dependencies
    self.defines = config.defines
    self.post_build_commands = config.post_build_commands
    self.build_flags = config.build_flags

    for i, src in pairs(self.sources) do
        src.files = configFilePath .. src.files
        self.sources[i] = src
    end
    for i, f in pairs(self.inc_dirs) do
        self.inc_dirs[i] = configFilePath .. f
    end
    for i, f in pairs(self.pub_inc_dirs) do
        self.pub_inc_dirs[i] = configFilePath .. f
    end
end

function ProjectConfig:set_name(name)
    self.name = name
end

function ProjectConfig:get_name()
    return self.name
end

function ProjectConfig:set_group(group)
    self.group = group
end

function ProjectConfig:get_group()
    return self.group
end

function ProjectConfig:add_dependencies(deps)
    a = table.deepcopy(self.dependencies)
    a[#a+1] = deps
    self.dependencies = a
end

function ProjectConfig:add_external_headers(header_path)
    a = table.deepcopy(self.external_headers)
    a[#a+1] = header_path
    self.external_headers = a
end

function ProjectConfig:add_external_lib_dir(lib_dir)
    a = table.deepcopy(self.external_lib_dir)
    a[#a+1] = lib_dir
    self.external_lib_dir = a
end

function ProjectConfig:add_external_lib(lib)
    a = table.deepcopy(self.external_libs)
    a[#a+1] = lib
    self.external_libs = a
end

function ProjectConfig:add_define(define)
    a = table.deepcopy(self.defines)
    a[#a+1] = define
    self.defines = a
end

function ProjectConfig:add_enabled_define(define)
    a = table.deepcopy(self.enabled_defines)
    a[#a+1] = define
    self.enabled_defines = a
end

function ProjectConfig:generate(all_projects, build_dir, proj_name)

    project(proj_name)
    do
        location(build_dir .. platform .. "/%{prj.name}")
        objdir ("!" .. buildDir .. platform .. "/int-%{cfg.shortname}/%{prj.name}/")
        kind(self.kind)
        language(self.language)
        
        proj_inc_dirs = table.join(self.pub_inc_dirs, self.inc_dirs, self.external_headers)
        proj_defines = self.defines
        for i, dep in pairs(self.dependencies) do
            if type(dep) == "table" then
                filter(dep.filter)
                -- if the kind is None, don't add link (e.g glm)
                if all_projects[dep.name].kind ~= "None" then
                    links(dep.name)
                end
                includedirs(all_projects[dep.name].pub_inc_dirs)
                defines(all_projects[dep.name].enabled_defines)
                filter({})
            else
                -- if the kind is None, don't add link (e.g glm)
                if all_projects[dep].kind ~= "None" then
                    links(dep)
                end
                includedirs(all_projects[dep].pub_inc_dirs)
                defines(all_projects[dep].enabled_defines)
            end
        end

        for i, lnk in pairs(self.external_libs) do
            if type(lnk) == "table" then
                filter(lnk.filter)
                links(lnk.name)
                filter({})
            else
                links(lnk)
            end
        end

        for i, src in pairs(self.sources) do
            -- print("\t\t"..src.files)
            if src.filter ~= nil and src.filter ~= "" then
                filter(src.filter)
            end
            files(src.files)
            if src.vpath ~= nil then
                vpaths({[src.vpath] = src.files})
            end
            filter({})
        end
        includedirs(proj_inc_dirs)
        libdirs(self.external_lib_dir)
        if (proj_defines ~= nil and (proj_defines ~= "" or type(proj_defines) == "table")) then
            for i,j in pairs(proj_defines) do
                if type(j) == "table" then
                    filter(j.filter)
                    defines(j.name)
                    filter({})
                elseif(j ~= "") then
                    defines(j)
                end
            end
        end
        for i,j in pairs(self.build_flags) do
            if _G[i] ~= nil then
                _G[i](j)
            else
                print("undefined function " .. i)
            end
        end
    end
end