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
        defines {
            defs
        }
        links {
            lnk
        }
        if userFunc then
            userFunc()
        end
end

group("g-test")
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
        
group("glad")
    project("glad")
        location("build/" .. platform .. "/%{prj.name}")
        kind "staticLib"
        language "c"
        
        files {
            "vendor/glad/include/glad/glad.h",
            "vendor/glad/include/glad/glad_wgl.h",
            "vendor/glad/include/KHR/khrplatform.h",
            "vendor/glad/src/glad.c",
            "vendor/glad/src/glad_wgl.c"
        }
        
        includedirs {
            "vendor/glad/include/",
            "vendor/glad/include/glad"
        }
        
group("libs")
    setupPrj("utils", "staticLib",
        {"libs/utils/include/**.hpp", "libs/utils/src/**.hpp", "libs/utils/src/**.cpp"}, -- files/filePath
        {"libs/utils/include/"}, -- include paths
        "", -- defines
        nil, -- links
        function() -- userFunc
            vpaths { ["utils/*"] = "libs/utils/src/**.hpp" }
            vpaths { ["utils/*"] = "libs/utils/src/**.cpp" }
            vpaths { ["utils/*"] = "libs/utils/include/**.hpp" }
        end  -- userFunc
    )

    setupPrj("script", "staticLib",
        {"libs/script/include/**.hpp", "libs/script/scr/**.hpp", "libs/script/src/**.cpp"}, -- files/FilePath
        {"libs/script/include/", "libs/utils/include/"}, -- includePaths
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
            vpaths { ["physics/*"] = "swizzle/physics/**.hpp" }
            vpaths { ["physics/*"] = "swizzle/physics/**.cpp" }
            vpaths { ["physics/*"] = "swizzle/include/physics/**.hpp" }

            filter "system:windows"
                defines
                {
                    "SW_WINDOWS",
                }
        end -- userFunc
    )

group("Engine")

    setupPrj("swizzle", "sharedLib",
        {"swizzle/include/**.hpp", "swizzle/engine_src/**.hpp", "swizzle/engine_src/**.cpp"}, -- files/filePath
        {"swizzle/include/", "swizzle/engine_src/", os.getenv("VULKAN_SDK") .. "/Include", "vendor/glm/include", "vendor/stb/include", "swizzle", "libs/utils/include"}, -- includePaths
        {"__MODULE__=\"SW_ENGINE\"", "SWIZZLE_DLL", "SWIZZLE_DLL_EXPORT"}, -- defines
        { "utils", "script", "physics", "vulkan-1", "shaderc_combined", "Xinput" }, -- links
        function() 
            vpaths { ["engine_src/*"] = "swizzle/engine_src/**.hpp" }
            vpaths { ["engine_src/*"] = "swizzle/engine_src/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }

            libdirs { os.getenv("VULKAN_SDK") .. "/Lib" }

            filter "system:windows"
                defines
                {
                    "SW_WINDOWS",
                }
        end
    )

group("Tests")
    setupPrj("scriptTest", "consoleApp",
        {"Vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "tests/script/**.cpp"}, -- files/filePath
        {"Vendor/google-test/googletest-master/googletest/include",
         "Vendor/google-test/googletest-master/googlemock/include" , "libs/script/include", "libs/script/src"}, -- include paths
        "", -- defines
        {"script", "google-test"}, -- links
        function() -- userFunc
            vpaths { ["*"] = "tests/**.hpp" }
            vpaths { ["*"] = "tests/**.cpp" }
        end -- userFunc
    )

    setupPrj("physicsTest", "consoleApp",
        {"Vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "tests/physics/**.cpp"}, -- files/filePath
        {"Vendor/google-test/googletest-master/googletest/include" , "swizzle"}, -- include paths
        "", -- defines
        {"physics", "google-test"}, -- links
        function() -- userFunc
            vpaths { ["*"] = "tests/**.hpp" }
            vpaths { ["*"] = "tests/**.cpp" }
        end -- userFunc
    )

    setupPrj("utilsTest", "consoleApp",
        {"Vendor/google-test/googletest-master/googletest/src/gtest_main.cc", "tests/utils/**.cpp"}, -- files/filePath
        {"Vendor/google-test/googletest-master/googletest/include" , "libs/utils/include"}, -- include paths
        "", -- defines
        {"utils", "google-test"}, -- links
        function() -- userFunc
            vpaths { ["*"] = "tests/**.hpp" }
            vpaths { ["*"] = "tests/**.cpp" }
        end -- userFunc
    )

group("Apps")
    setupPrj("sandbox", "consoleApp",
        {"swizzle/include/**.hpp", "sandbox/**.hpp", "sandbox/**.cpp"}, -- files/filePath
        {"sandbox/", "swizzle/include/", "vendor/glm/include", "libs/utils/include"}, -- include paths
        {"SWIZZLE_DLL"}, -- defines
        {"swizzle", "utils"}, -- links
        function() -- userFunc
            vpaths { ["sandbox/*"] = "sandbox/**.hpp" }
            vpaths { ["sandbox/*"] = "sandbox/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("modelConverter", "consoleApp",
        {"swizzle/include/**.hpp", "modelConverter/**.hpp", "modelConverter/**.cpp"}, -- files/filePath
        {"modelConverter/", "swizzle/include/", "vendor/glm/include", "libs/utils/include"}, -- include paths
        {"SWIZZLE_DLL"}, -- defines
        {"swizzle", "utils"}, -- links
        function() -- userFunc
            vpaths { ["modelConverter/*"] = "modelConverter/**.hpp" }
            vpaths { ["modelConverter/*"] = "modelConverter/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("scriptSandbox", "consoleApp",
        {"scriptSandbox/**.hpp", "scriptSandbox/**.cpp"}, -- files/filePath
        {"scriptSandbox/", "libs/script/include", "libs/utils/include"}, -- include paths
        {}, -- defines
        {"utils", "script"}, -- links
        function() -- userFunc
            vpaths { ["scriptSandbox/*"] = "scriptSandbox/**.hpp" }
            vpaths { ["scriptSandbox/*"] = "scriptSandbox/**.cpp" }

            -- vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )