include "build_scripts/workspace_funcs.lua"
include "build_scripts/project_funcs.lua"

setupWorkspace()

projectConfig = loadWorkspaceConfig("projectConfig.json")

print(table.tostring(projectConfig))

-- vulkan sdk
vulkanIncludeDirs = {os.getenv("VULKAN_SDK") .. "/Include", os.getenv("VULKAN_SDK") .. "/include"}
vulkanLibDirs = {os.getenv("VULKAN_SDK") .. "/Lib", os.getenv("VULKAN_SDK") .. "/lib"}

-- glm include dirs
glmIncludeDirs = "vendor/glm/include"

-- common include dirs
commonIncludeDirs = "libs/common/include"

projects = {}

-- projects
projects["utils"] = loadProjectConfig("libs/utils/utils.json")
projects["script"] = loadProjectConfig("libs/script/script.json")
projects["swm"] = loadProjectConfig("libs/swm/swm.json")
projects["physics"] = loadProjectConfig("libs/physics/physics.json")
projects["swizzle"] = loadProjectConfig("swizzle/swizzle.json")
-- vendor
projects["google-test"] = loadProjectConfig("vendor/google-test/gtest.json")
projects["imgui"] = loadProjectConfig("vendor/imgui/imgui.json")
projects["optick"] = loadProjectConfig("vendor/optick/optick.json")
projects["stb"] = loadProjectConfig("vendor/stb/stb.json")
-- tests
projects["utilsTest"] = loadProjectConfig("libs/utils/utilsTests.json")
projects["scriptTest"] = loadProjectConfig("libs/script/scriptTests.json")
projects["swmTest"] = loadProjectConfig("libs/swm/swmTests.json")
-- apps
projects["game"] = loadProjectConfig("apps/game.json")
projects["sandbox"] = loadProjectConfig("apps/sandbox.json")
projects["modelConverter"] = loadProjectConfig("apps/modelConverter.json")

addDependencies(projects["utilsTest"], {"utils", "google-test"})
addDependencies(projects["scriptTest"], {"script", "google-test"})
addDependencies(projects["swmTest"], {"swm", "google-test", "utils"})
addDependencies(projects["swm"], {"utils"})
addDependencies(projects["swizzle"], {"swm", "utils", "script", "physics", "optick", "imgui", "stb"})

addDependencies(projects["game"], {"swizzle", "utils", "optick"})
addDependencies(projects["sandbox"], {"swizzle", "imgui", "utils", "optick"})
addDependencies(projects["modelConverter"], {"swizzle", "utils", "optick"})

addExternalHeaders(projects["swm"], glmIncludeDirs)
addExternalHeaders(projects["swizzle"], glmIncludeDirs)
addExternalHeaders(projects["game"], glmIncludeDirs)
addExternalHeaders(projects["sandbox"], glmIncludeDirs)
addExternalHeaders(projects["modelConverter"], glmIncludeDirs)

addExternalHeaders(projects["swm"], commonIncludeDirs)
addExternalHeaders(projects["swizzle"], commonIncludeDirs)

addExternalHeaders(projects["game"], commonIncludeDirs)
addExternalHeaders(projects["sandbox"], commonIncludeDirs)
addExternalHeaders(projects["modelConverter"], commonIncludeDirs)

addExternalHeaders(projects["scriptTest"], commonIncludeDirs)
addExternalHeaders(projects["swmTest"], commonIncludeDirs)

addExternalHeaders(projects["swizzle"], vulkanIncludeDirs)
addExternalLibDir(projects["swizzle"], vulkanLibDirs)

addExternalHeaders(projects["optick"], vulkanIncludeDirs)
addExternalLibDir(projects["optick"], vulkanLibDirs)

addDefine(projects["swizzle"], {"__MODULE__=\"SW_ENGINE\"", "SWIZZLE_DLL", "SWIZZLE_DLL_EXPORT"})

addDefine(projects["sandbox"], "SWIZZLE_DLL")
addDefine(projects["game"], "SWIZZLE_DLL")
addDefine(projects["modelConverter"], "SWIZZLE_DLL")

if os.target() == "windows" then
    addExternalLib(projects["optick"], "vulkan-1")
    addExternalLib(projects["swizzle"], "vulkan-1")
    addExternalLib(projects["swizzle"], "Xinput")
    addDefine(projects["swizzle"], "SW_WINDOWS")
elseif os.target() == "linux" then
    addExternalLib(projects["optick"], "vulkan")
    addExternalLib(projects["swizzle"], "vulkan")
    addDefine(projects["swizzle"], "SW_LINUX_XLIB")
end

group("vendor")
generateProject(projects, "imgui")
generateProject(projects, "optick")
generateProject(projects, "stb")

group("libs")
generateProject(projects, "utils")
generateProject(projects, "script")
generateProject(projects, "swm")
generateProject(projects, "physics")

group("engine")
generateProject(projects, "swizzle")

if not projectConfig.disableApps then
    group("apps")
    generateProject(projects, "game")
    generateProject(projects, "sandbox")
    generateProject(projects, "modelConverter")
end

if not projectConfig.disableTests then
    group("tests")
    generateProject(projects, "google-test")
    generateProject(projects, "utilsTest")
    generateProject(projects, "scriptTest")
    generateProject(projects, "swmTest")
end

if not os.isfile("projectConfig.json") then
    storeWorkspaceConfig("projectConfig.json", projectConfig)
end

-- print(table.tostring(projects, 2))