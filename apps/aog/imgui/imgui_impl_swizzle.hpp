#ifndef IMGUI_IMPL_SWIZZLE_HPP
#define IMGUI_IMPL_SWIZZLE_HPP

/* Include files */

#include <imgui/imgui.h>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

// Called by user code
IMGUI_IMPL_API bool ImGui_ImplSwizzle_Init(common::Resource<swizzle::gfx::GfxContext> ctx,
                                           common::Resource<swizzle::core::SwWindow> window);
IMGUI_IMPL_API void ImGui_ImplSwizzle_Shutdown();
IMGUI_IMPL_API void ImGui_ImplSwizzle_NewFrame(common::Resource<swizzle::core::SwWindow> window);

IMGUI_IMPL_API common::Resource<swizzle::gfx::FrameBuffer> ImGui_ImplSwizzle_GetFramebuffer();
IMGUI_IMPL_API void ImGui_ImplSwizzle_SetMaterial(common::Resource<swizzle::gfx::Material> mat);
IMGUI_IMPL_API common::Resource<swizzle::gfx::Texture> ImGui_ImplSwizzle_GetFrameBufferTexture();

IMGUI_IMPL_API void ImGui_ImplSwizzle_RenderDrawData(ImDrawData* draw_data,
                                                     common::Unique<swizzle::gfx::DrawCommandTransaction>& dTrans);
IMGUI_IMPL_API void ImGui_ImplSwizzle_UploadFontTexture(common::Unique<swizzle::gfx::CommandTransaction>& trans);

IMGUI_IMPL_API common::Resource<swizzle::gfx::Material> ImGui_ImplSwizzle_CreateMaterial(common::Resource<swizzle::gfx::GfxContext> ctx);

#endif
