include "build_scripts/workspace_funcs.lua"
include "build_scripts/project_funcs.lua"


projectConfig = loadWorkspaceConfig("projectConfig.json")
setupWorkspace(projectConfig.buildDir)

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
projects["physics"] = loadProjectConfig("libs/physics/physics.json")
projects["swizzle"] = loadProjectConfig("swizzle/swizzle.json")
projects["swm"] = loadProjectConfig("swizzle/module_swm/swm.json")
-- vendor
projects["google-test"] = loadProjectConfig("vendor/google-test/gtest.json")
projects["imgui"] = loadProjectConfig("vendor/imgui/imgui.json")
projects["optick"] = loadProjectConfig("vendor/optick/optick.json")
projects["stb"] = loadProjectConfig("vendor/stb/stb.json")
-- tests
projects["utilsTest"] = loadProjectConfig("libs/utils/utilsTests.json")
projects["scriptTest"] = loadProjectConfig("libs/script/scriptTests.json")
projects["swmTest"] = loadProjectConfig("swizzle/module_swm/swmTests.json")
-- apps
projects["sandbox"] = loadProjectConfig("apps/sandbox.json")
projects["aog"] = loadProjectConfig("apps/aog.json")
projects["modelConverter"] = loadProjectConfig("apps/modelConverter.json")

addDependencies(projects["utilsTest"], {"utils", "google-test"})
addDependencies(projects["scriptTest"], {"script", "google-test"})
addDependencies(projects["swmTest"], {"swm", "google-test", "utils"})
addDependencies(projects["swm"], {"utils"})
addDependencies(projects["swizzle"], {"swm", "utils", "script", "physics", "optick", "imgui", "stb"})

addDependencies(projects["sandbox"], {"swizzle", "imgui", "utils", "optick"})
addDependencies(projects["aog"], {"swizzle", "imgui", "utils", "optick", "stb"})
addDependencies(projects["modelConverter"], {"swizzle", "utils", "optick"})

addExternalHeadersProjectList(projects, {"swm", "swmTest", "swizzle", "sandbox", "aog", "modelConverter"}, glmIncludeDirs)

addExternalHeadersProjectList(projects, {"swm", "swizzle", "sandbox", "aog", "modelConverter", "scriptTest", "swmTest"}, commonIncludeDirs)

addExternalHeaders(projects["swizzle"], vulkanIncludeDirs)
addExternalLibDir(projects["swizzle"], vulkanLibDirs)

addExternalHeaders(projects["optick"], vulkanIncludeDirs)
addExternalLibDir(projects["optick"], vulkanLibDirs)

addDefine(projects["swizzle"], {"__MODULE__=\"SW_ENGINE\"", "SWIZZLE_DLL", "SWIZZLE_DLL_EXPORT"})

addDefine(projects["sandbox"], "SWIZZLE_DLL")
addDefine(projects["aog"], "SWIZZLE_DLL")
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
generateProject(projects, projectConfig.buildDir, "imgui")
generateProject(projects, projectConfig.buildDir, "optick")
generateProject(projects, projectConfig.buildDir, "stb")

group("libs")
generateProject(projects, projectConfig.buildDir, "utils")
generateProject(projects, projectConfig.buildDir, "script")
generateProject(projects, projectConfig.buildDir, "swm")
generateProject(projects, projectConfig.buildDir, "physics")

group("engine")
generateProject(projects, projectConfig.buildDir, "swizzle")

if not projectConfig.disableApps then
    group("apps")
    generateProject(projects, projectConfig.buildDir, "sandbox")
    generateProject(projects, projectConfig.buildDir, "aog")
    generateProject(projects, projectConfig.buildDir, "modelConverter")
end

if not projectConfig.disableTests then
    group("tests")
    generateProject(projects, projectConfig.buildDir, "google-test")
    generateTestProject(projects, projectConfig.buildDir, "utilsTest")
    generateProject(projects, projectConfig.buildDir, "scriptTest")
    generateTestProject(projects, projectConfig.buildDir, "swmTest", "swm")
end

if not os.isfile("projectConfig.json") then
    storeWorkspaceConfig("projectConfig.json", projectConfig)
end

os.mkdir(".vscode")
generateVSCodeProjectSettings(projects, ".vscode/c_cpp_properties.json")
