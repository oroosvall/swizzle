
/* Include files */

#include "Compositor.hpp"

#include <swizzle/core/Logging.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Compositor::Compositor(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Swapchain> swap)
    : mCtx(ctx)
    , mLayers()
{
    RenderLayer rl (RenderLayerType::RenderLayer_Swapchain);
    rl.mSwapchain = swap;
    mLayers[0u] = rl;
}

common::Resource<swizzle::gfx::Shader> Compositor::createShader(U32 layerIndex, const swizzle::gfx::ShaderAttributeList& attribs)
{
    common::Resource<swizzle::gfx::Shader> shader = nullptr;
    auto& layer = mLayers[layerIndex];
    if (layer.mType == RenderLayerType::RenderLayer_Swapchain)
    {
        shader = mCtx->createShader(mLayers[layerIndex].mSwapchain, attribs);
    }
    else if (layer.mType == RenderLayerType::RenderLayer_FrameBuffer)
    {
        shader = mCtx->createShader(mLayers[layerIndex].mFrameBuffer, attribs);
    }
    else
    {
        LOG_ERROR("Render layer was invalid");
    }

    return shader;
}


/* Class Protected Function Definition */

/* Class Private Function Definition */
