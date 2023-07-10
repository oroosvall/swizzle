
#pragma once

/* Include files */

#include <imgui/imgui.h>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

IMGUI_IMPL_API bool ImGui_ImplSwizzle_Init(common::Resource<swizzle::gfx::GfxDevice> dev,
                                           common::Resource<swizzle::core::SwWindow> window);
IMGUI_IMPL_API void ImGui_ImplSwizzle_Shutdown();
IMGUI_IMPL_API void ImGui_ImplSwizzle_NewFrame(common::Resource<swizzle::core::SwWindow> window, DeltaTime dt);

IMGUI_IMPL_API common::Resource<swizzle::gfx::FrameBuffer> ImGui_ImplSwizzle_GetFramebuffer();
IMGUI_IMPL_API void ImGui_ImplSwizzle_SetMaterial(common::Resource<swizzle::gfx::Material> mat);
IMGUI_IMPL_API common::Resource<swizzle::gfx::Texture> ImGui_ImplSwizzle_GetFrameBufferTexture();

IMGUI_IMPL_API void ImGui_ImplSwizzle_RenderDrawData(ImDrawData* draw_data,
                                                     common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans);
IMGUI_IMPL_API void ImGui_ImplSwizzle_UploadFontTexture(common::Unique<swizzle::gfx::CommandTransaction>& trans);
