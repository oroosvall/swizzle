#ifndef COMPOSITOR_HPP
#define COMPOSITOR_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <unordered_map>

/* Defines */

/* Typedefs/enums */

enum class RenderLayerType
{
    RenderLayer_Invalid,
    RenderLayer_Swapchain,
    RenderLayer_FrameBuffer,
};

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct RenderLayer
{
    RenderLayer()
        : mType(RenderLayerType::RenderLayer_Invalid)
        , mSwapchain(nullptr)
    {
    }

    RenderLayer(RenderLayerType type)
        : mType(type)
        , mSwapchain(nullptr)
    {
    }

    ~RenderLayer()
    {
        if (mType == RenderLayerType::RenderLayer_Swapchain)
        {
            mSwapchain.reset();
        }
        else if (mType == RenderLayerType::RenderLayer_FrameBuffer)
        {
            mFrameBuffer.reset();
        }
        else
        {
            // do nothing
        }
    }

    void operator=(RenderLayer& org)
    {
        if (mType == RenderLayerType::RenderLayer_Swapchain)
        {
            mSwapchain.reset();
        }
        else if (mType == RenderLayerType::RenderLayer_FrameBuffer)
        {
            mFrameBuffer.reset();
        }
        else
        {
            // do nothing
        }

        mType = org.mType;
        if (org.mType == RenderLayerType::RenderLayer_Swapchain)
        {
            mSwapchain = org.mSwapchain;
        }
        else if (org.mType == RenderLayerType::RenderLayer_FrameBuffer)
        {
            mFrameBuffer = org.mFrameBuffer;
        }
        else
        {
            mSwapchain.reset();
        }
    }

    RenderLayerType mType;
    union
    {
        common::Resource<swizzle::gfx::Swapchain> mSwapchain;
        common::Resource<swizzle::gfx::FrameBuffer> mFrameBuffer;
    };
};

/* Class Declaration */

class Compositor
{
public:
    Compositor(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Swapchain> swap,
               common::Resource<swizzle::gfx::FrameBuffer> glowTexture);

    common::Resource<swizzle::gfx::Shader> createShader(U32 layerIndex,
                                                        const swizzle::gfx::ShaderAttributeList& attribs);

private:
    common::Resource<swizzle::gfx::GfxContext> mCtx;
    std::unordered_map<U32, RenderLayer> mLayers;
};

/* Function Declaration */

#endif
