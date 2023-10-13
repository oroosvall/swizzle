include "build_scripts/options.lua"
include "build_scripts/workspace_funcs.lua"
include "build_scripts/project_funcs.lua"

proj_cfg_file = _OPTIONS["projectConfig"]
project_config = load_workspace_config(_WORKING_DIR .. "/" .. proj_cfg_file)

print(project_config.workspace.name)
setup_workspace(project_config.workspace)

projects = {}

result, errorCode = os.outputof("git describe --always --dirty=-d")
s = string.format(
[[
#include <swizzle/swizzle.hpp>

namespace swizzle
{
    const SwChar* VersionString()
    {
        return "%s";
    }
}
]], result)
io.writefile("swizzle/engine_src/version.cpp", s)

print(result)

print("Loading projects")
for prj_name, prj_cfg in pairs(project_config.projects) do
    prj = Project:new(nil)
    prj:load(prj_name, prj_cfg, project_config.inc_dirs, project_config.lib_dirs)

    if prj:is_loaded() then
        projects[prj:get_name()] = prj:get_project()
        if prj:has_tests() then
            projects[prj:get_name().."-test"] = prj:get_tests()
        end
    else
        print("Problem loading project: " .. prj_cfg)
    end
end

print("Generating...")
for name, project in pairs(projects) do
    print("\t" .. project:get_name())
    group(project:get_group())
    project:generate(projects, _WORKING_DIR .. "/" .. project_config.workspace.build_dir, name)
end

if not os.isfile("projectConfig.json") then
    store_workspace_config("projectConfig.json", project_config)
end

os.mkdir(".vscode")
-- generateVSCodeProjectSettings(projects, ".vscode/c_cpp_properties.json")
