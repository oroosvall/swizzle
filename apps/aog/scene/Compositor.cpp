
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

Compositor::Compositor(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::gfx::Swapchain> swap,
                       common::Resource<swizzle::gfx::FrameBuffer> glowTexture)
    : mCtx(ctx)
    , mLayers()
{
    RenderLayer rl(RenderLayerType::RenderLayer_Swapchain);
    rl.mSwapchain = swap;
    mLayers[0u] = rl;

    RenderLayer fb(RenderLayerType::RenderLayer_FrameBuffer);
    fb.mFrameBuffer = glowTexture;
    mLayers[1u] = fb;
}

common::Resource<swizzle::gfx::Shader> Compositor::createShader(U32 layerIndex,
                                                                const swizzle::gfx::ShaderAttributeList& attribs)
{
    common::Resource<swizzle::gfx::Shader> shader = nullptr;
    auto& layer = mLayers[layerIndex];
    if (layer.mType == RenderLayerType::RenderLayer_Swapchain)
    {
        shader =
            mCtx->createShader(mLayers[layerIndex].mSwapchain, swizzle::gfx::ShaderType::ShaderType_Graphics, attribs);
    }
    else if (layer.mType == RenderLayerType::RenderLayer_FrameBuffer)
    {
        shader = mCtx->createShader(mLayers[layerIndex].mFrameBuffer, swizzle::gfx::ShaderType::ShaderType_Graphics,
                                    attribs);
    }
    else
    {
        LOG_ERROR("Render layer was invalid");
    }

    return shader;
}

common::Resource<swizzle::gfx::Shader> Compositor::createComputeShader(U32 layerIndex,
                                                                       const swizzle::gfx::ShaderAttributeList& attribs)
{
    common::Resource<swizzle::gfx::Shader> shader = nullptr;
    auto& layer = mLayers[layerIndex];
    if (layer.mType == RenderLayerType::RenderLayer_Swapchain)
    {
        shader =
            mCtx->createShader(mLayers[layerIndex].mSwapchain, swizzle::gfx::ShaderType::ShaderType_Compute, attribs);
    }
    else if (layer.mType == RenderLayerType::RenderLayer_FrameBuffer)
    {
        shader = mCtx->createShader(mLayers[layerIndex].mFrameBuffer, swizzle::gfx::ShaderType::ShaderType_Compute,
                                    attribs);
    }
    else
    {
        LOG_ERROR("Render layer was invalid");
    }

    return shader;
}

common::Resource<swizzle::gfx::Shader> Compositor::createMeshShader(U32 layerIndex,
                                                                    const swizzle::gfx::ShaderAttributeList& attribs)
{
    common::Resource<swizzle::gfx::Shader> shader = nullptr;
    auto& layer = mLayers[layerIndex];
    if (layer.mType == RenderLayerType::RenderLayer_Swapchain)
    {
        shader = mCtx->createShader(mLayers[layerIndex].mSwapchain, swizzle::gfx::ShaderType::ShaderType_Mesh, attribs);
    }
    else if (layer.mType == RenderLayerType::RenderLayer_FrameBuffer)
    {
        shader =
            mCtx->createShader(mLayers[layerIndex].mFrameBuffer, swizzle::gfx::ShaderType::ShaderType_Mesh, attribs);
    }
    else
    {
        LOG_ERROR("Render layer was invalid");
    }

    return shader;
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
