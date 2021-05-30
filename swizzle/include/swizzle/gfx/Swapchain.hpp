#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <common/Common.hpp>

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

        virtual void clearFrameCounter() = 0;
        virtual U64 getFrameCounter() = 0;

        virtual common::Resource<Shader> createShader(const ShaderAttributeList& attribs) = 0;

        virtual void prepare() = 0;
        virtual void present() = 0;

    };
}

#endif