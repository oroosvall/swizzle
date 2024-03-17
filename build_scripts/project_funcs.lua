include "project_config.lua"
include "helpers.lua"

Project = {
    name = nil,
    prj = nil,
    prj_test = nil
}

function Project:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Project:get_name()
    return self.name
end

function Project:load(name, prj, inc_dir, lib_dir)
    print("\t" .. name .. ": " .. prj.source)
    self.name = name
    self.prj = ProjectConfig:new(nil)
    self.prj:set_name(self.name)
    self.prj:set_group(prj.group)
    self.prj:load_from_json(prj.source)

    if prj.external_include_dirs ~= nil then
        for i, inc in pairs(prj.external_include_dirs) do
            self.prj:add_external_headers(resolve_dir(inc, inc_dir[inc]))
        end
    end
    if prj.external_lib_dirs ~= nil then
        for i, dir in pairs(prj.external_lib_dirs) do
            self.prj:add_external_lib_dir(resolve_dir(dir, lib_dir[dir]))
        end
    end
    if prj.external_libs ~= nil then
        for i, lib in pairs(prj.external_libs) do
            self.prj:add_external_lib(lib)
        end
    end
    if prj.defines ~= nil then
        for i, def in pairs(prj.defines) do
            self.prj:add_define(def)
        end
    end
    if prj.enabled_defines ~= nil then
        for i, def in pairs(prj.enabled_defines) do
            self.prj:add_enabled_define(def)
        end
    end

    if prj.dependencies ~= nil then
        for i, dep in pairs(prj.dependencies) do
            self.prj:add_dependencies(dep)
        end
    end

    if prj.test_source ~= nil then
        print("\t" .. name .. "-test: " .. prj.test_source)
        self.prj_test = ProjectConfig:new(nil)
        self.prj_test:set_name(self.name .. "-test")
        self.prj_test:set_group("test")
        self.prj_test:load_from_json(prj.test_source)
        if prj.external_include_dirs ~= nil then
            for i, inc in pairs(prj.external_include_dirs) do
                self.prj_test:add_external_headers(resolve_dir(inc, inc_dir[inc]))
            end
        end
        self.prj_test:add_dependencies(name)

        if prj.test_dependencies ~= nil then
            for i, dep in pairs(prj.test_dependencies) do
                self.prj_test:add_dependencies(dep)
            end
        end
    end
end

function Project:is_loaded()
    return self.prj ~= nil
end

function Project:has_tests()
    return self.prj_test ~= nil
end

function Project:get_project()
    return self.prj
end

function Project:get_tests()
    return self.prj_test
end