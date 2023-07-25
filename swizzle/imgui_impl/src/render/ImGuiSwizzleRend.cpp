
/* Include files */

#include "ImGuiSwizzleRend.hpp"
#include "ImGuiShader.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

struct WindowRenderBuffers
{
    common::Resource<swizzle::gfx::GfxBuffer> mVertexBuffer;
    common::Resource<swizzle::gfx::GfxBuffer> mIndexBuffer;
};

struct ImGui_ImplSwizzle_RenderViewportData
{
    ImGui_ImplSwizzle_RenderViewportData()
        : mRenderBuffers()
    {
    }

    WindowRenderBuffers mRenderBuffers;
};

struct MainResources
{
    // Font data
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Texture> mFontTexture;
    SwBool mFontUploaded;
    common::Resource<swizzle::gfx::Material> mFontMaterial;
};

struct ImGui_ImplSwizzle_RendData
{
    common::Resource<swizzle::gfx::GfxDevice> mDevice;
    common::Resource<ImGuiSwizzleRenderTarget> mRenderTarget;

    MainResources mMainResources;
};

/* Static Function Declaration */

static void ImGui_ImplSwizzle_CreateRendererObjects();
static void ImGui_ImplSwizzle_DestroyRendererObjects();

static void ImGui_ImplVulkan_SetupRenderState(ImDrawData* draw_data,
                                              common::Resource<swizzle::gfx::Shader> shad,
                                              common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans,
                                              WindowRenderBuffers* rb, int fb_width, int fb_height);

static void ImGui_ImplSwizzle_Rend_CreateRenderBuffers(ImGuiViewport* viewport);

/* Static Function Definition */

static ImGui_ImplSwizzle_RendData* ImGui_ImplSwizzle_GetRendBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplSwizzle_RendData*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

static void ImGui_ImplSwizzle_CreateRendererObjects()
{
    ImGui_ImplSwizzle_RendData* bd = ImGui_ImplSwizzle_GetRendBackendData();
    ImGuiIO& io = ImGui::GetIO();

    common::Resource<swizzle::gfx::GfxDevice> dev = bd->mDevice;

    // Shader attributes
    swizzle::gfx::ShaderAttributeList attributeList = {};

    attributeList.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, IM_OFFSETOF(ImDrawVert, pos)},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, IM_OFFSETOF(ImDrawVert, uv)},
        {0u, swizzle::gfx::ShaderAttributeDataType::r8b8g8a8_unorm, IM_OFFSETOF(ImDrawVert, col)},
    };

    attributeList.mBufferInput = { {swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(ImDrawVert)} };
    attributeList.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler,
         swizzle::gfx::Count(1u),
         {swizzle::gfx::StageType::fragmentStage}},
    };
    attributeList.mPushConstantSize = sizeof(float) * 4u;
    attributeList.mEnableDepthTest = false;
    attributeList.mEnableDepthWrite = false;
    attributeList.mEnableBlending = true;
    attributeList.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    // Create shader
    bd->mMainResources.mShader =
        bd->mRenderTarget->createShader(dev, swizzle::gfx::ShaderType::ShaderType_Graphics, attributeList);
    uint32_t vertSize = 0u;
    uint32_t* vert = GetImguiVertexShader(vertSize);
    uint32_t fragSize = 0u;
    uint32_t* frag = GetImguiFragmentShader(fragSize);
    bd->mMainResources.mShader->loadVertFragMemory(vert, vertSize, frag, fragSize, GetShaderProperties());

    // Create font
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    bd->mMainResources.mFontTexture = dev->createTexture(width, height, 4, false);
    bd->mMainResources.mFontUploaded = false;
    bd->mMainResources.mFontMaterial =
        dev->createMaterial(bd->mMainResources.mShader, swizzle::gfx::SamplerMode::SamplerModeClamp);
    bd->mMainResources.mFontMaterial->setDescriptorTextureResource(0, bd->mMainResources.mFontTexture);

    io.Fonts->SetTexID((ImTextureID)&bd->mMainResources.mFontMaterial);
}

static void ImGui_ImplSwizzle_DestroyRendererObjects()
{
    // Cleanup viewport buffers
    ImGuiPlatformIO& platformIo = ImGui::GetPlatformIO();
    for (int n = 0; n < platformIo.Viewports.Size; n++)
    {
        if (ImGui_ImplSwizzle_RenderViewportData* vd =
            (ImGui_ImplSwizzle_RenderViewportData*)platformIo.Viewports[n]->RendererUserData)
        {
            vd->mRenderBuffers.mVertexBuffer.reset();
            vd->mRenderBuffers.mIndexBuffer.reset();
        }
    }

    ImGui_ImplSwizzle_RendData* bd = ImGui_ImplSwizzle_GetRendBackendData();

    // reset font
    bd->mMainResources.mFontMaterial.reset();
    bd->mMainResources.mFontTexture.reset();
    bd->mMainResources.mShader.reset();
    bd->mMainResources.mFontUploaded = false;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->SetTexID(0);
}

void ImGui_ImplVulkan_SetupRenderState(ImDrawData* drawData,
                                       common::Resource<swizzle::gfx::Shader> shad,
                                       common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans,
                                       WindowRenderBuffers* rb, int fbWidth, int fbHeight)
{
    dTrans->bindShader(shad);
    dTrans->setViewport((U32)fbWidth, (U32)fbHeight);

    struct PushData
    {
        float scale[2];
        float translate[2];
    } data = {};

    data.scale[0] = 2.0f / drawData->DisplaySize.x;
    data.scale[1] = (-2.0f) / drawData->DisplaySize.y;
    data.translate[0] = (-1.0f) - (drawData->DisplayPos.x * data.scale[0]);
    data.translate[1] = (1.0f) - (drawData->DisplayPos.y * data.scale[1]);

    dTrans->setShaderConstant(shad, (U8*)&data, sizeof(data));
    if (drawData->TotalVtxCount > 0)
    {
        dTrans->bindVertexBuffer(rb->mVertexBuffer);
        dTrans->bindIndexBuffer(rb->mIndexBuffer, sizeof(ImDrawIdx) == 2);
    }
}

static void ImGui_ImplSwizzle_Rend_CreateRenderBuffers(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_RendData* bd = ImGui_ImplSwizzle_GetRendBackendData();
    ImGui_ImplSwizzle_RenderViewportData* vd = (ImGui_ImplSwizzle_RenderViewportData*)viewport->RendererUserData;

    vd->mRenderBuffers.mVertexBuffer = bd->mDevice->createBuffer(swizzle::gfx::GfxBufferType::Vertex,
                                                                 swizzle::gfx::GfxMemoryArea::DeviceLocalHostVisible);
    vd->mRenderBuffers.mIndexBuffer = bd->mDevice->createBuffer(swizzle::gfx::GfxBufferType::Index,
                                                                swizzle::gfx::GfxMemoryArea::DeviceLocalHostVisible);
}

/* Function Definition */

bool ImGui_ImplSwizzle_Rend_Init(common::Resource<swizzle::gfx::GfxDevice> dev,
                                  common::Resource<ImGuiSwizzleRenderTarget> renderTarget,
                                  common::Resource<swizzle::core::SwWindow>(pfnGetWindow)(ImGuiViewport*))
{
    UNUSED_ARG(pfnGetWindow);
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");
    IM_ASSERT(dev != nullptr && "Invalid device");
    IM_ASSERT(renderTarget != nullptr && "Must provide valid rendertarget");
    //IM_ASSERT(pfnGetWindow != nullptr && "Must provide function to get viewport window");

    // Setup rendering backend
    ImGui_ImplSwizzle_RendData* bd = IM_NEW(ImGui_ImplSwizzle_RendData)();
    bd->mDevice = dev;
    bd->mRenderTarget = renderTarget;
    //bd->mPfnGetWindow = pfnGetWindow;
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_swizzle_renderer";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing
    // for large meshes.
    //io.BackendFlags |=
    //    ImGuiBackendFlags_RendererHasViewports; // We can create multi-viewports on the Renderer side (optional)

    ImGui_ImplSwizzle_CreateRendererObjects();

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    mainViewport->RendererUserData = IM_NEW(ImGui_ImplSwizzle_RenderViewportData)();
    ImGui_ImplSwizzle_Rend_CreateRenderBuffers(mainViewport);

    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //    ImGui_ImplSwizzle_Rend_InitInterface();

    return true;
}

void ImGui_ImplSwizzle_Rend_Shutdown()
{
    ImGui_ImplSwizzle_RendData* bd = ImGui_ImplSwizzle_GetRendBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    // Destroy resources
    ImGui_ImplSwizzle_DestroyRendererObjects();

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    if (ImGui_ImplSwizzle_RenderViewportData* vd =
        (ImGui_ImplSwizzle_RenderViewportData*)mainViewport->RendererUserData)
    {
        IM_DELETE(vd);
    }
    mainViewport->RendererUserData = nullptr;

    //ImGui_ImplSwizzle_Rend_ShudownInterface();

    bd->mDevice.reset();
    bd->mRenderTarget.reset();

    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports);
    IM_DELETE(bd);
}

void ImGui_ImplSwizzle_Rend_UploadFontTexture(common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    ImGui_ImplSwizzle_RendData* bd = ImGui_ImplSwizzle_GetRendBackendData();
    if (!bd->mMainResources.mFontUploaded)
    {
        auto data =
            bd->mDevice->createBuffer(swizzle::gfx::GfxBufferType::UniformBuffer, swizzle::gfx::GfxMemoryArea::Host);

        unsigned char* pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        U64 size = U64(width) * U64(height) * 4ull;
        data->setBufferData(pixels, size, 4ull);

        swizzle::gfx::TextureDimensions dims{};
        dims.mHeight = height;
        dims.mWidth = width;
        dims.mMipLevels = 1u;
        dims.mLayers = 1u;

        trans->copyBufferToTexture(bd->mMainResources.mFontTexture, data, dims);
        bd->mMainResources.mFontUploaded = true;
    }
}

void ImGui_ImplSwizzle_Rend_DrawData(ImDrawData* drawData,
                                      common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer
    // coordinates)
    int fbWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int fbHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    if (fbWidth <= 0 || fbHeight <= 0)
        return;

    ImGui_ImplSwizzle_RendData* bd = ImGui_ImplSwizzle_GetRendBackendData();

    // Allocate array to store enough vertex/index buffers. Each unique viewport gets its own storage.
    ImGui_ImplSwizzle_RenderViewportData* rvd =
        (ImGui_ImplSwizzle_RenderViewportData*)drawData->OwnerViewport->RendererUserData;
    IM_ASSERT(rvd != nullptr);

    common::Resource<swizzle::gfx::Shader> shad = nullptr; // rvd->mWindow.mShader;
    if (!shad)
    {
        shad = bd->mMainResources.mShader;
    }


    WindowRenderBuffers* rb = &rvd->mRenderBuffers;

    if (drawData->TotalVtxCount > 0)
    {
        // Create or resize the vertex/index buffers
        size_t vertex_size = drawData->TotalVtxCount * sizeof(ImDrawVert);
        size_t index_size = drawData->TotalIdxCount * sizeof(ImDrawIdx);

        rb->mVertexBuffer->setStride(sizeof(ImDrawVert));
        rb->mIndexBuffer->setStride(sizeof(ImDrawIdx));

        // Upload vertex/index data into a single contiguous GPU buffer
        ImDrawVert* vtx_dst = (ImDrawVert*)rb->mVertexBuffer->mapMemory(vertex_size);

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = drawData->CmdLists[n];
            memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            vtx_dst += cmd_list->VtxBuffer.Size;
        }

        rb->mVertexBuffer->unmapMemory();

        ImDrawIdx* idx_dst = (ImDrawIdx*)rb->mIndexBuffer->mapMemory(index_size);

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = drawData->CmdLists[n];
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            idx_dst += cmd_list->IdxBuffer.Size;
        }

        rb->mIndexBuffer->unmapMemory();
    }

    ImGui_ImplVulkan_SetupRenderState(drawData, shad, dTrans, rb, fbWidth, fbHeight);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = drawData->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer
                // to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplVulkan_SetupRenderState(drawData, shad, dTrans, rb, fbWidth, fbHeight);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
                                (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
                                (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                if (clip_min.x < 0.0f)
                {
                    clip_min.x = 0.0f;
                }
                if (clip_min.y < 0.0f)
                {
                    clip_min.y = 0.0f;
                }
                if (clip_max.x > fbWidth)
                {
                    clip_max.x = (float)fbWidth;
                }
                if (clip_max.y > fbHeight)
                {
                    clip_max.y = (float)fbHeight;
                }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                S32 sciX = (S32)clip_min.x;
                S32 sciY = (S32)clip_min.y;

                S32 sciW = (S32)(clip_max.x - clip_min.x);
                S32 sciH = (S32)(clip_max.y - clip_min.y);

                dTrans->setScissor(sciX, sciY, (U32)sciW, (U32)sciH);

                common::Resource<swizzle::gfx::Material>* material =
                    (common::Resource<swizzle::gfx::Material>*)pcmd->TextureId;

                if constexpr (sizeof(ImTextureID) < sizeof(ImU64))
                {
                    // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit. Do a flaky
                    // check that other textures haven't been used.
                    IM_ASSERT(pcmd->TextureId == (ImTextureID)&bd->mMainResources.mFontMaterial);
                    material = &bd->mMainResources.mFontMaterial;
                }
                dTrans->bindMaterial(shad, *material);

                // Draw
                dTrans->drawIndexedNoBind(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset,
                                          pcmd->VtxOffset + global_vtx_offset);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    dTrans->setScissor(0, 0, (U32)fbWidth, (U32)fbWidth);
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
