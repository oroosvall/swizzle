
/* Include files */

#include "ImGuiSwzzle.hpp"
#include "platform/ImGuiSwizzlePlf.hpp"
#include "render/ImGuiSwizzleRend.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

bool ImGui_ImplSwizzle_Init(common::Resource<swizzle::core::SwWindow> window,
                            common::Resource<swizzle::gfx::GfxDevice> dev,
                            common::Resource<ImGuiSwizzleRenderTarget> renderTarget)
{
    bool ok = ImGui_ImplSwizzle_Plf_Init(window);
    ok &= ImGui_ImplSwizzle_Rend_Init(dev, renderTarget, ImGui_ImplSwizzle_Plf_GetViewportWindow);
    return ok;
}

void ImGui_ImplSwizzle_Shutdown()
{
    ImGui_ImplSwizzle_Rend_Shutdown();
    ImGui_ImplSwizzle_Plf_Shutdown();
}

void ImGui_ImplSwizzle_NewFrame(DeltaTime dt)
{
    ImGui_ImplSwizzle_Plf_NewFrame(dt);
}

void ImGui_ImplSwizzle_UploadFontTexture(common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    ImGui_ImplSwizzle_Rend_UploadFontTexture(trans);
}

void ImGui_ImplSwizzle_DrawData(ImDrawData* drawData, common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans)
{
    ImGui_ImplSwizzle_Rend_DrawData(drawData, dTrans);
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
