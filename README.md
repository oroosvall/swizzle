
# Swizzle

A project intended to explore the Vulkan API.

# Dependencies

`VULKAN_SDK` must be defined as environment variable.

If the SDK is installed under `C:/VulkanSDK/x.x.x.x/` then the `VULKAN_SDK` should point to the same path

Don't forget to checkout submodules
> git submodule update --init

# Build

## Windows
> setup.bat

in project root, this requires visual studio 2019 or later with the vs2019 platform toolset and the toolchain version of v142. Project is targeting c++17.

## Linux

run setup by calling premake in vendor directory

> ./vendor/premake/premake5 gmake2 premake5.lua

navigate to build directory

> cd build/gmake2

> make sandbox 