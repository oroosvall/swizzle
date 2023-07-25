
#pragma once

/* Include files */

#include <imgui/imgui.h>
#include <swizzle/core/Event.hpp>
#include <swizzle/gfx/Context.hpp>

#include <variant>

/* Defines */

/* Typedefs/enums */

enum class RenderTarget
{
    None,
    Swapchain,
    FrameBuffer
};

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/// <summary>
/// The ImGuiSwizzleRenderTarget is wrapper to user provided rendertarget
/// This is either a swapchain or a frambuffer
/// </summary>
class ImGuiSwizzleRenderTarget
{
public:
    ImGuiSwizzleRenderTarget(common::Resource<swizzle::gfx::FrameBuffer> fbo)
        : mRenderTarget(RenderTarget::FrameBuffer)
        , mThing(fbo)
    {
    }

    ImGuiSwizzleRenderTarget(common::Resource<swizzle::gfx::Swapchain> swp)
        : mRenderTarget(RenderTarget::Swapchain)
        , mThing(swp)
    {
    }

    ~ImGuiSwizzleRenderTarget() {}

    common::Resource<swizzle::gfx::Shader> createShader(common::Resource<swizzle::gfx::GfxDevice> dev,
                                                        swizzle::gfx::ShaderType type,
                                                        swizzle::gfx::ShaderAttributeList& attribs)
    {
        if (mRenderTarget == RenderTarget::FrameBuffer)
        {
            common::Resource<swizzle::gfx::FrameBuffer>* fbo =
                std::get_if<common::Resource<swizzle::gfx::FrameBuffer>>(&mThing);
            return dev->createShader(*fbo, type, attribs);
        }
        else if (mRenderTarget == RenderTarget::Swapchain)
        {
            common::Resource<swizzle::gfx::Swapchain>* swp =
                std::get_if<common::Resource<swizzle::gfx::Swapchain>>(&mThing);
            return dev->createShader(*swp, type, attribs);
        }
        else
        {
        }
        return nullptr;
    }

    common::Unique<swizzle::gfx::DrawCommandTransaction>
        beginRenderPass(common::Resource<swizzle::gfx::CommandBuffer> cmd,
                        common::Unique<swizzle::gfx::CommandTransaction> trans)
    {
        if (mRenderTarget == RenderTarget::FrameBuffer)
        {
            common::Resource<swizzle::gfx::FrameBuffer>* fbo =
                std::get_if<common::Resource<swizzle::gfx::FrameBuffer>>(&mThing);
            return cmd->beginRenderPass(*fbo, std::move(trans));
        }
        else if (mRenderTarget == RenderTarget::Swapchain)
        {
            common::Resource<swizzle::gfx::Swapchain>* swp =
                std::get_if<common::Resource<swizzle::gfx::Swapchain>>(&mThing);
            return cmd->beginRenderPass(*swp, std::move(trans));
        }

        return nullptr;
    }

    common::Resource<swizzle::gfx::Texture> getTexture()
    {
        if (mRenderTarget == RenderTarget::FrameBuffer)
        {
            common::Resource<swizzle::gfx::FrameBuffer>* fbo =
                std::get_if<common::Resource<swizzle::gfx::FrameBuffer>>(&mThing);
            return (*fbo)->getColorAttachment(0u);
        }
        return nullptr;
    }

private:
    RenderTarget mRenderTarget;
    std::variant<common::Resource<swizzle::gfx::FrameBuffer>, common::Resource<swizzle::gfx::Swapchain>> mThing;
};

/* Function Declaration */

IMGUI_IMPL_API bool ImGui_ImplSwizzle_Init(common::Resource<swizzle::core::SwWindow> window,
                                           common::Resource<swizzle::gfx::GfxDevice> dev,
                                           common::Resource<ImGuiSwizzleRenderTarget> renderTarget);

IMGUI_IMPL_API void ImGui_ImplSwizzle_Shutdown();

IMGUI_IMPL_API void ImGui_ImplSwizzle_NewFrame(DeltaTime dt);
IMGUI_IMPL_API void ImGui_ImplSwizzle_UploadFontTexture(common::Unique<swizzle::gfx::CommandTransaction>& trans);
IMGUI_IMPL_API void ImGui_ImplSwizzle_DrawData(ImDrawData* drawData,
                                               common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans);
