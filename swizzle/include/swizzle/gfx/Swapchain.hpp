#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/Resource.hpp>

#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Shader.hpp>

namespace swizzle::gfx
{

    enum class VSyncTypes
    {
        vSyncOff,
        vSyncOn,
        vSyncAdaptive,
    };

    class Swapchain
    {
    public:
        virtual ~Swapchain() {}

        virtual SwBool isVsyncModeSupported(VSyncTypes sync) const = 0;
        virtual void setVsync(VSyncTypes sync) = 0;

        virtual void setClearColor(ClearColor color) = 0;

        virtual core::Resource<Shader> createShader(ShaderAttributeList attribs) = 0;

        virtual void prepare() = 0;
        virtual void present() = 0;

    };
}

#endif