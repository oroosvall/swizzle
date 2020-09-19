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
        }

        includedirs {
            "vendor/google-test/googletest-master/googletest/include/",
            "vendor/google-test/googletest-master/googletest/"
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
        
group("Utils")
setupPrj("utils", "staticLib",
    {"swizzle/include/utils/**.hpp", "swizzle/utils/**.hpp", "swizzle/utils/**.cpp"}, -- files/filePath
    {"swizzle/include/"}, -- include paths
    "", -- defines
    nil, -- links
    function() -- userFunc
        vpaths { ["utils/*"] = "swizzle/utils/**.hpp" }
        vpaths { ["utils/*"] = "swizzle/utils/**.cpp" }
        vpaths { ["utils/*"] = "swizzle/include/utils/**.hpp" }
    end  -- userFunc
)

group("Engine")
    setupPrj("script", "staticLib",
        {"swizzle/script/**.hpp","swizzle/script/**.cpp",}, -- files/FilePath
        {"swizzle/include/"}, -- includePaths
        {""}, -- defines
        {"utils"}, -- links
        function() -- userFunc
            vpaths { ["script/*"] = "swizzle/script/**.hpp" }
            vpaths { ["script/*"] = "swizzle/script/**.cpp" }

            vpaths { ["script/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )

    setupPrj("physics", "staticLib",
        {"swizzle/include/physics/**.hpp", "swizzle/physics/**.hpp", "swizzle/physics/**.cpp"}, -- files/FilePath
        {"swizzle/include/", "vendor/glm/include"}, -- includePaths
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

    setupPrj("swizzle", "sharedLib",
        {"swizzle/include/**.hpp", "swizzle/engine_src/**.hpp", "swizzle/engine_src/**.cpp"}, -- files/filePath
        {"swizzle/include/", "swizzle/engine_src/", os.getenv("VULKAN_SDK") .. "/Include", "vendor/glm/include", "vendor/stb/include", "swizzle"}, -- includePaths
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
        {"Vendor/google-test/googletest-master/googletest/include" , "swizzle"}, -- include paths
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
        {"Vendor/google-test/googletest-master/googletest/include" , "swizzle/include"}, -- include paths
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
        {"sandbox/" , "swizzle/include/", "vendor/glm/include"}, -- include paths
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
        {"modelConverter/" , "swizzle/include/", "vendor/glm/include"}, -- include paths
        {"SWIZZLE_DLL"}, -- defines
        {"swizzle", "utils"}, -- links
        function() -- userFunc
            vpaths { ["modelConverter/*"] = "modelConverter/**.hpp" }
            vpaths { ["modelConverter/*"] = "modelConverter/**.cpp" }

            vpaths { ["include/*"] = "swizzle/include/**.hpp" }
        end -- userFunc
    )