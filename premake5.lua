include "build_scripts/options.lua"
include "build_scripts/workspace_funcs.lua"
include "build_scripts/project_funcs.lua"

print(_ACTION)
proj_cfg_file = _OPTIONS["projectConfig"]
print(proj_cfg_file)
project_config = load_workspace_config(proj_cfg_file)

print(project_config.workspace.name)
setup_workspace(project_config.workspace)

projects = {}

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
    project:generate(projects, project_config.workspace.build_dir, name)
end

if not os.isfile("projectConfig.json") then
    store_workspace_config("projectConfig.json", project_config)
end

os.mkdir(".vscode")
-- generateVSCodeProjectSettings(projects, ".vscode/c_cpp_properties.json")
