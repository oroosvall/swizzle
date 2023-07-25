
#pragma once

/* Include files */

#include <imgui/imgui.h>
#include <swizzle/core/Window.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

bool ImGui_ImplSwizzle_Plf_Init(common::Resource<swizzle::core::SwWindow> window);
void ImGui_ImplSwizzle_Plf_Shutdown();
void ImGui_ImplSwizzle_Plf_NewFrame(DeltaTime dt);
common::Resource<swizzle::core::SwWindow> ImGui_ImplSwizzle_Plf_GetViewportWindow(ImGuiViewport* viewport);
