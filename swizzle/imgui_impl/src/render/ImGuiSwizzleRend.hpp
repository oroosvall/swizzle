
#pragma once

/* Include files */

#include <ImGuiSwzzle.hpp>
#include <imgui/imgui.h>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

bool ImGui_ImplSwizzle_Rend_Init(common::Resource<swizzle::gfx::GfxDevice> dev,
                                 common::Resource<ImGuiSwizzleRenderTarget> renderTarget,
                                 common::Resource<swizzle::core::SwWindow>(pfnGetWindow)(ImGuiViewport*));
void ImGui_ImplSwizzle_Rend_Shutdown();
void ImGui_ImplSwizzle_Rend_UploadFontTexture(common::Unique<swizzle::gfx::CommandTransaction>& trans);
void ImGui_ImplSwizzle_Rend_DrawData(ImDrawData* drawData,
                                     common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans);
