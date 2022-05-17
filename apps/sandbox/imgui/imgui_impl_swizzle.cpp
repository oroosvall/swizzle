
/* Include files */

#include "imgui_impl_swizzle.hpp"
#include "ImGuiTranslateKey.hpp"


#include <swizzle/core/Input.hpp>
#include <swizzle/core/Platform.hpp>
#include <swizzle/core/Event.hpp>
#include <swizzle/core/WindowEvents.hpp>

#include <optick/optick.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

struct ImGui_ImplSwizzle_Data
{
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Texture> mFontTexture;
    common::Resource<swizzle::gfx::Material> mFontMaterial;

    common::Resource<swizzle::gfx::Buffer> mVertexBuffer;
    common::Resource<swizzle::gfx::Buffer> mIndexBuffer;

    common::Resource<swizzle::gfx::FrameBuffer> mImGuiFbo;
    common::Resource<swizzle::gfx::Material> mImGuiMat;
};

class ImGuiInputCallback : public swizzle::EventHandler<swizzle::core::WindowEvent>
{
protected:
    virtual void publishEvent(const swizzle::core::WindowEvent& evt) override;
};


/* Static Variables */

//-----------------------------------------------------------------------------
// SHADERS
//-----------------------------------------------------------------------------

// glsl_shader.vert, compiled with:
// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
/*
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;
    gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
}
*/
static uint32_t __glsl_shader_vert_spv[] =
{
    0x07230203,0x00010000,0x0008000a,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
};

// glsl_shader.frag, compiled with:
// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
/*
#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
}
*/
static uint32_t __glsl_shader_frag_spv[] =
{
    0x07230203,0x00010000,0x0008000a,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
};

//    color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//    color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//    color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
//    color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//    color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

static const SwChar* properties =
"sourceBlend=[SRC_ALPHA,SRC_ALPHA]\n\
destinationBlend=[ONE_MINUS_SRC_ALPHA,ONE]\n\
blending=enabled\n\
depthTest=disabled\n\
colorBlend=ADD\n\
alphaBlend=ADD\n\
";

ImGuiInputCallback cb;

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

ImGui_ImplSwizzle_Data* ImGui_ImplSwizzle_GetBackendData();

void ImGui_ImplVulkan_SetupRenderState(ImDrawData* draw_data, common::Resource<swizzle::gfx::CommandBuffer> command_buffer, int fb_width, int fb_height);

/* Class Public Function Definition */

void ImGuiInputCallback::publishEvent(const swizzle::core::WindowEvent& evt)
{
    OPTICK_EVENT("ImGuiInputCallback::publishEvent");
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();

    auto evtType = evt.getEventType();
    switch (evtType)
    {
    case swizzle::core::WindowEventType::CharacterTypeEvent :
    {
        swizzle::core::CharacterEvent& e = (swizzle::core::CharacterEvent&)evt;
        if (e.mCodePoint > 0 && e.mCodePoint < 0x10000)
        {
            io.AddInputCharacterUTF16((ImWchar16)e.mCodePoint);
        }
        break;
    }
    case swizzle::core::WindowEventType::KeyboardInputEvent:
    {
        swizzle::core::InputEvent& e = (swizzle::core::InputEvent&)evt;
        if (e.mFromKeyboard)
        {
            io.AddKeyEvent(fromScanCode(e.mKey), e.mPressed);
            puts(swizzle::core::GetKeyText(e.mKey));
        }
        else
        {
            io.AddMouseButtonEvent(e.mKey - 1, e.mPressed);
        }
        break;
    }
    case swizzle::core::WindowEventType::MouseMoveEvent:
    {
        swizzle::core::MouseMoveEvent& e = (swizzle::core::MouseMoveEvent&)evt;
        io.AddMousePosEvent((float) e.mX, (float)e.mY);
        break;
    }
    case swizzle::core::WindowEventType::MouseScrollEvent:
    {
        swizzle::core::MouseScrollEvent& e = (swizzle::core::MouseScrollEvent&)evt;
        io.AddMouseWheelEvent((float)e.mScrollX, (float)e.mScrollY);
        break;
    }
    case swizzle::core::WindowEventType::ResizeEvent:
    {
        swizzle::core::WindowResizeEvent& e = (swizzle::core::WindowResizeEvent&)evt;
        bd->mImGuiFbo->resize(e.mWidth, e.mHeight);
        bd->mImGuiMat->setDescriptorTextureResource(0u, bd->mImGuiFbo->getColorAttachment(0u));
    }
    default:
        break;
    }
}

bool ImGui_ImplSwizzle_Init(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::core::SwWindow> window)
{
    ImGuiIO& io = ImGui::GetIO();
    window->addEventListener(&cb);

    // Setup backend capabilities flags
    ImGui_ImplSwizzle_Data* bd = IM_NEW(ImGui_ImplSwizzle_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_vulkan";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

    IM_ASSERT(ctx != nullptr);

    swizzle::gfx::FrameBufferCreateInfo info{};
    info.mDepthType = swizzle::gfx::FrameBufferDepthType::DepthNone;
    info.mNumColAttach = 1u;
    info.mSwapCount = 3u;
    window->getSize(info.mWidth, info.mHeight);

    bd->mImGuiFbo = ctx->createFramebuffer(info);
    bd->mImGuiFbo->setColorAttachmentClearColor(0u, { 0.0f, 0.0f, 0.0f, 0.0f });

    swizzle::gfx::ShaderAttributeList attributeList = {};

    attributeList.mAttributes = {
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec2f, IM_OFFSETOF(ImDrawVert, pos)},
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec2f, IM_OFFSETOF(ImDrawVert, uv) },
        { 0u, swizzle::gfx::ShaderAttributeDataType::r8b8g8a8_unorm, IM_OFFSETOF(ImDrawVert, col) },
    };

    attributeList.mBufferInput = {
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(ImDrawVert) }
    };
    attributeList.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler, swizzle::gfx::Count(1u), {swizzle::gfx::StageType::fragmentStage}},
    };
    attributeList.mPushConstantSize = sizeof(float) * 4u;
    attributeList.mEnableDepthTest = false;
    attributeList.mEnableBlending = true;
    attributeList.mPoints = false;

    bd->mShader = bd->mImGuiFbo->createShader(attributeList);
    bd->mShader->loadVertFragMemory(__glsl_shader_vert_spv, sizeof(__glsl_shader_vert_spv), __glsl_shader_frag_spv, sizeof(__glsl_shader_frag_spv), properties);

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    bd->mFontTexture = ctx->createTexture(width, height, 4, pixels);
    bd->mFontMaterial = ctx->createMaterial(bd->mShader);
    bd->mFontMaterial->setDescriptorTextureResource(0, bd->mFontTexture);

    bd->mVertexBuffer = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);
    bd->mIndexBuffer = ctx->createBuffer(swizzle::gfx::BufferType::Index);

    io.Fonts->SetTexID((ImTextureID)&bd->mFontMaterial);

    return true;
}

void ImGui_ImplSwizzle_Shutdown()
{
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();
    IM_ASSERT(bd != NULL && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    bd->mIndexBuffer.reset();
    bd->mVertexBuffer.reset();
    bd->mFontMaterial.reset();
    bd->mFontTexture.reset();
    bd->mShader.reset();
    bd->mImGuiFbo.reset();

    io.BackendRendererName = NULL;
    io.BackendRendererUserData = NULL;
    IM_DELETE(bd);
}

void ImGui_ImplSwizzle_NewFrame(common::Resource<swizzle::core::SwWindow> window)
{
    ImGuiIO& io = ImGui::GetIO();

    U32 w, h;
    window->getSize(w, h);
    io.DisplaySize = ImVec2((float)w, (float)h);

    // Update OS mouse position
    if (io.WantSetMousePos)
    {
        S32 x;
        S32 y;
        swizzle::input::GetMousePosition(x, y);
        io.MousePos.x = (float)x;
        io.MousePos.y = (float)y;
    }
}

void ImGui_ImplSwizzle_BeginDraw(common::Resource<swizzle::gfx::CommandBuffer> command_buffer)
{
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();
    command_buffer->beginRenderPass(bd->mImGuiFbo);
}

void ImGui_ImplSwizzle_EndDraw(common::Resource<swizzle::gfx::CommandBuffer> command_buffer)
{
    command_buffer->endRenderPass();
}

void ImGui_ImplSwizzle_SetMaterial(common::Resource<swizzle::gfx::Material> mat)
{
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();
    mat->setDescriptorTextureResource(0u, bd->mImGuiFbo->getColorAttachment(0u));
    bd->mImGuiMat = mat;
}

common::Resource<swizzle::gfx::Texture> ImGui_ImplSwizzle_GetFrameBufferTexture()
{
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();
    return bd->mImGuiFbo->getColorAttachment(0u);
}

void ImGui_ImplSwizzle_RenderDrawData(ImDrawData* draw_data, common::Resource<swizzle::gfx::CommandBuffer> command_buffer)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();
    //ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;
    //if (pipeline == VK_NULL_HANDLE)
    //    pipeline = bd->Pipeline;

    //// Allocate array to store enough vertex/index buffers
    //ImGui_ImplVulkanH_WindowRenderBuffers* wrb = &bd->MainWindowRenderBuffers;
    //if (wrb->FrameRenderBuffers == NULL)
    //{
    //    wrb->Index = 0;
    //    wrb->Count = v->ImageCount;
    //    wrb->FrameRenderBuffers = (ImGui_ImplVulkanH_FrameRenderBuffers*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameRenderBuffers) * wrb->Count);
    //    memset(wrb->FrameRenderBuffers, 0, sizeof(ImGui_ImplVulkanH_FrameRenderBuffers) * wrb->Count);
    //}
    //IM_ASSERT(wrb->Count == v->ImageCount);
    //wrb->Index = (wrb->Index + 1) % wrb->Count;
    //ImGui_ImplVulkanH_FrameRenderBuffers* rb = &wrb->FrameRenderBuffers[wrb->Index];

    if (draw_data->TotalVtxCount > 0)
    {
        // Create or resize the vertex/index buffers
        size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
        size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

        bd->mVertexBuffer->setStride(sizeof(ImDrawVert));
        bd->mIndexBuffer->setStride(sizeof(ImDrawIdx));

        // Upload vertex/index data into a single contiguous GPU buffer
        ImDrawVert* vtx_dst = (ImDrawVert*)bd->mVertexBuffer->mapMemory(vertex_size);

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            vtx_dst += cmd_list->VtxBuffer.Size;
        }

        bd->mVertexBuffer->unmapMemory();

        ImDrawIdx* idx_dst = (ImDrawIdx*)bd->mIndexBuffer->mapMemory(index_size);

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            idx_dst += cmd_list->IdxBuffer.Size;
        }

        bd->mIndexBuffer->unmapMemory();

    }

    // Setup desired Vulkan state
    ImGui_ImplVulkan_SetupRenderState(draw_data, command_buffer, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplVulkan_SetupRenderState(draw_data, command_buffer, fb_width, fb_height);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                if (clip_min.x < 0.0f)
                {
                    clip_min.x = 0.0f;
                }
                if (clip_min.y < 0.0f)
                {
                    clip_min.y = 0.0f;
                }
                if (clip_max.x > fb_width)
                {
                    clip_max.x = (float)fb_width;
                }
                if (clip_max.y > fb_height)
                {
                    clip_max.y = (float)fb_height;
                }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                S32 sciX = (S32)clip_min.x;
                S32 sciY = (S32)clip_min.y;

                S32 sciW = (S32)(clip_max.x - clip_min.x);
                S32 sciH = (S32)(clip_max.y - clip_min.y);

                command_buffer->setScissor(sciX, sciY, sciW, sciH);

                common::Resource<swizzle::gfx::Material>* material = (common::Resource<swizzle::gfx::Material>*)pcmd->TextureId;

                if constexpr (sizeof(ImTextureID) < sizeof(ImU64))
                {
                    // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit. Do a flaky check that other textures haven't been used.
                    IM_ASSERT(pcmd->TextureId == (ImTextureID)&bd->mFontMaterial);
                    material = &bd->mFontMaterial;
                }
                command_buffer->bindMaterial(bd->mShader, *material);

                // Draw
                command_buffer->drawIndexedNoBind(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    command_buffer->setScissor(0, 0, (U32)fb_width, (U32)fb_height);
}

void ImGui_ImplSwizzle_UploadFontTexture(common::Resource<swizzle::gfx::CommandBuffer> command_buffer)
{
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();
    command_buffer->uploadTexture(bd->mFontTexture);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

ImGui_ImplSwizzle_Data* ImGui_ImplSwizzle_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplSwizzle_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}

void ImGui_ImplVulkan_SetupRenderState(ImDrawData* draw_data, common::Resource<swizzle::gfx::CommandBuffer> command_buffer, int fb_width, int fb_height)
{
    ImGui_ImplSwizzle_Data* bd = ImGui_ImplSwizzle_GetBackendData();

    command_buffer->bindShader(bd->mShader);
    command_buffer->setViewport((U32)fb_width, (U32)fb_height);

    struct PushData
    {
        float scale[2];
        float translate[2];
    } data = {};

    data.scale[0] = 2.0f / draw_data->DisplaySize.x;
    data.scale[1] = (-2.0f) / draw_data->DisplaySize.y;
    data.translate[0] = (-1.0f) - (draw_data->DisplayPos.x * data.scale[0]);
    data.translate[1] = (1.0f) - (draw_data->DisplayPos.y * data.scale[1]);

    command_buffer->setShaderConstant(bd->mShader, (U8*)&data, sizeof(data));
    if (draw_data->TotalVtxCount > 0)
    {
        command_buffer->bindVertexBuffer(bd->mVertexBuffer);
        command_buffer->bindIndexBuffer(bd->mIndexBuffer, sizeof(ImDrawIdx) == 2);
    }
}
