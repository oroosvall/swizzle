
# Swizzle

This project is a vulkan based game engine, with the purpose of exploring and learning the Vulkan API

# Setup

This project is depending on the vulkan SDK enviroment variable. `https://vulkan.lunarg.com/`

The project currently targets version 1.3.231.0 or higher of the SDK.

The following git repositories are added as submodules
* google-test
* glm
* portaudio

run
```bash
git submodule update --init --recursive
```

## Windows
Installing the vulkan sdk is enough to have the `VULKAN_SDK` environment variable set.
`VULKAN_SDK` must be defined as environment variable.

If the SDK is installed under `C:/VulkanSDK/x.x.x.x/` then the `VULKAN_SDK` should point to the same path.

run the `setup.bat`

## Linux
`VULKAN_SDK` environment variable should point to the source directory of the vulkan headers and the library location.

### Manual SDK source
source the sdk setup-env
On linux source the `setup-env.sh` before running the premake setup.

```bash
source 'path/to/vulkan_sdk/setup_env.sh'
./vendor/premake/premake5 gmake2
```

### Package manager provided sources
The vulkan sdk headers and library files are usually installed into the /usr directoy

```bash
export VULKAN_SDK=/usr
./vendor/premake/premake5 gmake2
```

# Features

Here are some of the Vulkan features the engine supports.

| Feature                    | Comment        |
|----------------------------|----------------|
| MeshShaders                | Simple support |
| Multiple Framebuffers      |                |
| Resource lifetime tracking |                |
| Multithreading             | No, not tested |

**Note:** list is uncomplete