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
IMGUI_IMPL_API bool ImGui_ImplSwizzle_Init(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::core::SwWindow> window);
IMGUI_IMPL_API void ImGui_ImplSwizzle_Shutdown();
IMGUI_IMPL_API void ImGui_ImplSwizzle_NewFrame(common::Resource<swizzle::core::SwWindow> window);

IMGUI_IMPL_API void ImGui_ImplSwizzle_BeginDraw(common::Resource<swizzle::gfx::CommandBuffer> command_buffer);
IMGUI_IMPL_API void ImGui_ImplSwizzle_EndDraw(common::Resource<swizzle::gfx::CommandBuffer> command_buffer);
IMGUI_IMPL_API void ImGui_ImplSwizzle_SetMaterial(common::Resource<swizzle::gfx::Material> mat);
IMGUI_IMPL_API common::Resource<swizzle::gfx::Texture> ImGui_ImplSwizzle_GetFrameBufferTexture();

IMGUI_IMPL_API void ImGui_ImplSwizzle_RenderDrawData(ImDrawData* draw_data, common::Resource<swizzle::gfx::CommandBuffer> command_buffer);
IMGUI_IMPL_API void ImGui_ImplSwizzle_UploadFontTexture(common::Resource<swizzle::gfx::CommandBuffer> command_buffer);

#endif
