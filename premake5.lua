include "workspace.lua"


function setupPrj(prjName, prjType, filePath, includePath, defs, lnk, userFunc)
    project(prjName)

        location("build/" .. platform .."/%{prj.name}")
        kind(prjType)
        language "c++"

        files {
            filePath
        }
        includedirs {
            includePath
        }
        if (defs ~= "" or type(defs) == "table") then
            for i,j in pairs(defs) do
                if(j ~= "") then
                    defines {j}
                end
            end
        end
        links {
            lnk
        }
        if userFunc then
            userFunc()
        end
end

extIncDirs = {}
extIncDirs['common'] = "libs/common/include"
extIncDirs['physics'] = "libs/physics/include"
extIncDirs['script'] = "libs/script/include"
extIncDirs['utils'] = "libs/utils/include"
extIncDirs['vk_sdk'] = {os.getenv("VULKAN_SDK") .. "/Include", os.getenv("VULKAN_SDK") .. "/include"}

group("vendor")
    project("google-test")
        location("build/" .. platform .."/%{prj.name}")
        kind "staticLib"
        language "c++"
        
        files {
            "vendor/google-test/googletest-master/googletest/include/**.h",
            "vendor/google-test/googletest-master/googletest/include/**.cc",
            "vendor/google-test/googletest-master/googletest/src/**.h",
            "vendor/google-test/googletest-master/googletest/src/gtest-all.cc",

            "vendor/google-test/googletest-master/googlemock/include/**.h",
            "vendor/google-test/googletest-master/googlemock/include/**.cc",
            "vendor/google-test/googletest-master/googlemock/src/**.h",
            "vendor/google-test/googletest-master/googlemock/src/gmock-all.cc",
        }

        includedirs {
            "vendor/google-test/googletest-master/googletest/include/",
            "vendor/google-test/googletest-master/googletest/",
            "vendor/google-test/googletest-master/googlemock/include/",
            "vendor/google-test/googletest-master/googlemock/"
        }
        
    -- project("glad")
    --     location("build/" .. platform .. "/%{prj.name}")
    --     kind "staticLib"
    --     language "c"
        
    --     files {
    --         "vendor/glad/include/glad/glad.h",
    --         "vendor/glad/include/glad/glad_wgl.h",
    --         "vendor/glad/include/KHR/khrplatform.h",
    --         "vendor/glad/src/glad.c",
    --         "vendor/glad/src/glad_wgl.c"
    --     }
        
    --     includedirs {
    --         "vendor/glad/include/",
    --         "vendor/glad/include/glad"
    --     }
        
    project("stb")
        location("build/" .. platform .. "/%{prj.name}")
        kind "staticLib"
        language "c"
        
        files {
            "vendor/stb/include/stb/*.h",
            "libs/vendor/stb/*.c"
        }
        
        includedirs {
            "vendor/stb/include/",
        }
        filter "system:linux"
            disablewarnings { "sign-compare", "unused-but-set-variable" }
        pic "On"

group("libs")
    setupPrj("utils", "staticLib",
        {"libs/utils/include/**.hpp", "libs/utils/src/**.hpp", "libs/utils/src/**.cpp"}, -- files/filePath
        {extIncDirs['utils']}, -- include paths
        "", -- defines
        nil, -- links
        function() -- userFunc
            vpaths { ["utils/*"] = "libs/utils/src/**.hpp" }
            vpaths { ["utils/*"] = "libs/utils/src/**.cpp" }
            vpaths { ["include/*"] = "libs/utils/include/**.hpp" }
        end  -- userFunc
    )

    setupPrj("script", "staticLib",
        {"libs/script/include/**.hpp", "libs/script/scr/**.hpp", "libs/script/src/**.cpp"}, -- files/FilePath
        {"libs/script/include/", extIncDirs['utils']}, -- includePaths
        {""}, -- defines
        {"utils"}, -- links
        function() -- userFunc
            vpaths { ["script/*"] = "swizzle/script/**.hpp" }
            vpaths { ["script/*"] = "swizzle/script/**.cpp" }

            vpaths { ["script/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("physics", "staticLib",
        {"libs/physics/include/**.hpp", "libs/physics/src/**.hpp", "libs/physics/src/**.cpp"}, -- files/FilePath
        {"libs/physics/include/", "vendor/glm/include"}, -- includePaths
        {""}, -- defines
        nil, -- links
        function() -- userFunc
            vpaths { ["physics/*"] = "lib/physics/**.hpp" }
            vpaths { ["physics/*"] = "lib/physics/**.cpp" }
            vpaths { ["physics/*"] = "lib/physics/include/**.hpp" }

            filter "system:windows"
                defines
                {
                    "SW_WINDOWS",
                }
        end -- userFunc
    )

    setupPrj("swm", "staticLib",
        {"libs/swm/include/**.hpp", "libs/swm/src/**.hpp", "libs/swm/src/**.cpp"}, -- files/FilePath
        {"libs/swm/include/", extIncDirs['utils'], "vendor/glm/include", extIncDirs['common']}, -- includePaths
        {""}, -- defines
        nil, -- links
        function() -- userFunc
            vpaths { ["swm/*"] = "lib/swm/**.hpp" }
            vpaths { ["swm/*"] = "lib/swm/**.cpp" }
            vpaths { ["swm/*"] = "lib/swm/include/**.hpp" }
        end -- userFunc
    )

group("Engine")

    setupPrj("swizzle", "sharedLib",
        {"swizzle/include/**.hpp", "swizzle/engine_src/**.hpp", "swizzle/engine_src/**.cpp", extIncDirs['common'].."/**.hpp"}, -- files/filePath
        {"swizzle/include/", "swizzle/engine_src/", extIncDirs['vk_sdk'], "vendor/glm/include", "vendor/stb/include", "swizzle", extIncDirs['utils'], extIncDirs['common'], "libs/swm/include/"}, -- includePaths
        {"__MODULE__=\"SW_ENGINE\"", "SWIZZLE_DLL", "SWIZZLE_DLL_EXPORT"}, -- defines
        { "utils", "script", "physics", "shaderc_combined", "stb", "swm" }, -- links
        function() 
            vpaths { ["engine_src/*"] = "swizzle/engine_src/**.hpp" }
            vpaths { ["engine_src/*"] = "swizzle/engine_src/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
            vpaths { ["include/*"] = extIncDirs['common'].."/**.hpp" }

            libdirs { os.getenv("VULKAN_SDK") .. "/Lib", os.getenv("VULKAN_SDK") .. "/lib" }

            filter "system:windows"
                defines
                {
                    "SW_WINDOWS",
                }
                links
                {
                    "vulkan-1",
                    "Xinput"
                }
            filter "system:linux"
                defines
                {
                    "SW_LINUX_XLIB",
                }
                links
                {
                    "vulkan"
                }
        end
    )

group("Tests")
    setupPrj("scriptTest", "consoleApp",
        {"vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "tests/script/**.cpp"}, -- files/filePath
        {"vendor/google-test/googletest-master/googletest/include",
         "vendor/google-test/googletest-master/googlemock/include" , "libs/script/include", "libs/script/src"}, -- include paths
        "", -- defines
        {"script", "google-test"}, -- links
        function() -- userFunc
            vpaths { ["*"] = "tests/**.hpp" }
            vpaths { ["*"] = "tests/**.cpp" }
        end -- userFunc
    )

    setupPrj("physicsTest", "consoleApp",
        {"vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "tests/physics/**.cpp"}, -- files/filePath
        {"vendor/google-test/googletest-master/googletest/include" , "swizzle"}, -- include paths
        "", -- defines
        {"physics", "google-test"}, -- links
        function() -- userFunc
            vpaths { ["*"] = "tests/**.hpp" }
            vpaths { ["*"] = "tests/**.cpp" }
        end -- userFunc
    )

    setupPrj("utilsTest", "consoleApp",
        {"vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "tests/utils/**.cpp"}, -- files/filePath
        {"vendor/google-test/googletest-master/googletest/include" , "libs/utils/include"}, -- include paths
        "", -- defines
        {"utils", "google-test"}, -- links
        function() -- userFunc
            vpaths { ["*"] = "tests/**.hpp" }
            vpaths { ["*"] = "tests/**.cpp" }
        end -- userFunc
    )

    setupPrj("swmTest", "consoleApp",
    {"vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "libs/swm/test/**.cpp"}, -- files/filePath
    {"vendor/google-test/googletest-master/googletest/include" , "libs/swm/include", "libs/swm/src", "libs/utils/include", "libs/common/include"}, -- include paths
    "", -- defines
    {"swm", "google-test"}, -- links
    function() -- userFunc
        vpaths { ["*"] = "tests/**.hpp" }
        vpaths { ["*"] = "tests/**.cpp" }
    end -- userFunc
)

group("Apps")
    setupPrj("sandbox", "consoleApp",
        {"swizzle/include/**.hpp", "apps/sandbox/**.hpp", "apps/sandbox/**.cpp"}, -- files/filePath
        {"apps/sandbox/", "swizzle/include/", "vendor/glm/include", "libs/utils/include", extIncDirs['common'], "libs/swm/include"}, -- include paths
        {"SWIZZLE_DLL"}, -- defines
        {"swizzle", "utils", "swm"}, -- links
        function() -- userFunc
            vpaths { ["sandbox/*"] = "apps/sandbox/**.hpp" }
            vpaths { ["sandbox/*"] = "apps/sandbox/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("modelConverter", "consoleApp",
        {"swizzle/include/**.hpp", "apps/modelConverter/**.hpp", "apps/modelConverter/**.cpp"}, -- files/filePath
        {"apps/modelConverter/", "swizzle/include/", "vendor/glm/include", "libs/utils/include", extIncDirs['common']}, -- include paths
        {"SWIZZLE_DLL"}, -- defines
        {"swizzle", "utils"}, -- links
        function() -- userFunc
            vpaths { ["modelConverter/*"] = "apps/modelConverter/**.hpp" }
            vpaths { ["modelConverter/*"] = "apps/modelConverter/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("scriptSandbox", "consoleApp",
        {"apps/scriptSandbox/**.hpp", "apps/scriptSandbox/**.cpp"}, -- files/filePath
        {"apps/scriptSandbox/", "libs/script/include", "libs/utils/include", extIncDirs['common']}, -- include paths
        {}, -- defines
        {"utils", "script"}, -- links
        function() -- userFunc
            vpaths { ["scriptSandbox/*"] = "apps/scriptSandbox/**.hpp" }
            vpaths { ["scriptSandbox/*"] = "apps/scriptSandbox/**.cpp" }

            -- vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("game", "consoleApp",
        {"swizzle/include/**.hpp", "apps/game/**.hpp", "apps/game/**.cpp"}, -- files/filePath
        {"apps/game/", "swizzle/include/", "vendor/glm/include", "libs/utils/include", extIncDirs['common']}, -- include paths
        {"SWIZZLE_DLL"}, -- defines
        {"swizzle", "utils"}, -- links
        function() -- userFunc
            vpaths { ["game/*"] = "apps/game/**.hpp" }
            vpaths { ["game/*"] = "apps/game/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )