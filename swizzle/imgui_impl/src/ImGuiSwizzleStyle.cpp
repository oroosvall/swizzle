

/* Include files */

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <fstream>
#include <string>
#include <unordered_map>

/* Defines */

/* Typedefs */

/* Structs/Classes */

struct StyleInfo
{
    std::unordered_map<ImGuiCol, ImVec4> mColors;
};

struct ImVec3
{
    float x, y, z;
    constexpr ImVec3()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {
    }

    constexpr ImVec3(float _x, float _y, float _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }
};

/* Static Variables */

StyleInfo gStyle;

ImVec3 gColWidgets{1.00f, 0.72f, 0.24f}; // widgets, e.g button { 0.26f, 0.59f, 0.98f };
ImVec3 gColSeparator{0.59f, 0.51f, 0.37f}; // Separator { 0.10f, 0.40f, 0.75f };
ImVec3 gColActive{1.00f, 0.63f, 0.00f}; // SliderGrab, Activated (pressed) button { 0.24f, 0.52f, 0.88f };
ImVec3 gColBackground{0.71f, 0.51f, 0.16f}; // Background
ImVec3 gColBorder{ 0.6f, 0.51f, 0.25f }; // Border

/* Static Function Declaration */

static ImVec4 FromVec3(const ImVec3& v, float w);

static void ConstructThemeSimple(const ImVec3& widgets, const ImVec3& separator, const ImVec3& active,
                                 const ImVec3& background, const ImVec3& border);

static void ImGui_ImplSwizzle_SetupStyle(const StyleInfo& styleInfo);

/* Static Function Definition */

static ImVec4 FromVec3(const ImVec3& v, float w)
{
    return ImVec4{v.x, v.y, v.z, w};
}

static void ConstructThemeSimple(const ImVec3& widgets, const ImVec3& separator, const ImVec3& active,
                                 const ImVec3& background, const ImVec3& border)
{
    // clang-format off
    gStyle.mColors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    gStyle.mColors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    gStyle.mColors[ImGuiCol_WindowBg]              = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    gStyle.mColors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    gStyle.mColors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    gStyle.mColors[ImGuiCol_Border]                = FromVec3(border, 0.50f);
    gStyle.mColors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    gStyle.mColors[ImGuiCol_FrameBg]               = FromVec3(background, 0.54f);
    gStyle.mColors[ImGuiCol_FrameBgHovered]        = FromVec3(widgets, 0.40f);
    gStyle.mColors[ImGuiCol_FrameBgActive]         = FromVec3(widgets, 0.67f);
    gStyle.mColors[ImGuiCol_TitleBg]               = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    gStyle.mColors[ImGuiCol_TitleBgActive]         = FromVec3(background, 1.00f);
    gStyle.mColors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    gStyle.mColors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    gStyle.mColors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    gStyle.mColors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    gStyle.mColors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    gStyle.mColors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    gStyle.mColors[ImGuiCol_CheckMark]             = FromVec3(widgets, 1.0f);
    gStyle.mColors[ImGuiCol_SliderGrab]            = FromVec3(active, 1.0f);
    gStyle.mColors[ImGuiCol_SliderGrabActive]      = FromVec3(widgets, 1.0f);
    gStyle.mColors[ImGuiCol_Button]                = FromVec3(widgets, 0.40f);
    gStyle.mColors[ImGuiCol_ButtonHovered]         = FromVec3(widgets, 1.0f);
    gStyle.mColors[ImGuiCol_ButtonActive]          = FromVec3(active, 1.0f);
    gStyle.mColors[ImGuiCol_Header]                = FromVec3(widgets, 0.31f);
    gStyle.mColors[ImGuiCol_HeaderHovered]         = FromVec3(widgets, 0.80f);
    gStyle.mColors[ImGuiCol_HeaderActive]          = FromVec3(widgets, 1.0f);
    gStyle.mColors[ImGuiCol_Separator]             = gStyle.mColors[ImGuiCol_Border];
    gStyle.mColors[ImGuiCol_SeparatorHovered]      = FromVec3(separator, 0.78f);
    gStyle.mColors[ImGuiCol_SeparatorActive]       = FromVec3(separator, 1.00f);
    gStyle.mColors[ImGuiCol_ResizeGrip]            = FromVec3(widgets, 0.20f);
    gStyle.mColors[ImGuiCol_ResizeGripHovered]     = FromVec3(widgets, 0.67f);
    gStyle.mColors[ImGuiCol_ResizeGripActive]      = FromVec3(widgets, 0.95f);
    gStyle.mColors[ImGuiCol_Tab]                   = ImLerp(gStyle.mColors[ImGuiCol_Header], gStyle.mColors[ImGuiCol_TitleBgActive], 0.80f);
    gStyle.mColors[ImGuiCol_TabHovered]            = gStyle.mColors[ImGuiCol_HeaderHovered];
    gStyle.mColors[ImGuiCol_TabActive]             = ImLerp(gStyle.mColors[ImGuiCol_HeaderActive], gStyle.mColors[ImGuiCol_TitleBgActive], 0.60f);
    gStyle.mColors[ImGuiCol_TabUnfocused]          = ImLerp(gStyle.mColors[ImGuiCol_Tab], gStyle.mColors[ImGuiCol_TitleBg], 0.80f);
    gStyle.mColors[ImGuiCol_TabUnfocusedActive]    = ImLerp(gStyle.mColors[ImGuiCol_TabActive], gStyle.mColors[ImGuiCol_TitleBg], 0.40f);
    gStyle.mColors[ImGuiCol_DockingPreview]        = gStyle.mColors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    gStyle.mColors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    gStyle.mColors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    gStyle.mColors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    gStyle.mColors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    gStyle.mColors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    gStyle.mColors[ImGuiCol_TableHeaderBg]         = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    gStyle.mColors[ImGuiCol_TableBorderStrong]     = ImVec4(0.31f, 0.31f, 0.35f, 1.00f); // Prefer using Alpha=1.0 here
    gStyle.mColors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f); // Prefer using Alpha=1.0 here
    gStyle.mColors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    gStyle.mColors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    gStyle.mColors[ImGuiCol_TextSelectedBg]        = FromVec3(widgets, 0.35f);
    gStyle.mColors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    gStyle.mColors[ImGuiCol_NavHighlight]          = FromVec3(widgets, 1.0f);
    gStyle.mColors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    gStyle.mColors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    gStyle.mColors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    // clang-format on
}

static void ImGui_ImplSwizzle_SetupStyle(const StyleInfo& styleInfo)
{
    ImGuiStyle& imStyle = ImGui::GetStyle();

    for (const auto& colPair : styleInfo.mColors)
    {
        imStyle.Colors[colPair.first] = colPair.second;
    }
}

/* Function Definition */

void ImGui_ImplSwizzle_SetTheme()
{
    ConstructThemeSimple(gColWidgets, gColSeparator, gColActive, gColBackground, gColBorder);
    ImGui_ImplSwizzle_SetupStyle(gStyle);
}

//void ImGui_ImplSwizzle_EditTheme()
//{
//    static bool simple = true;
//    ImGui::Begin("Theme Editor");
//    ImGui::Checkbox("simple", &simple);
//
//    if (simple)
//    {
//        bool upd = ImGui::ColorEdit3("Widgets", (float*)&gColWidgets, ImGuiColorEditFlags_NoInputs);
//        upd |= ImGui::ColorEdit3("Separator", (float*)&gColSeparator, ImGuiColorEditFlags_NoInputs);
//        upd |= ImGui::ColorEdit3("Active", (float*)&gColActive, ImGuiColorEditFlags_NoInputs);
//        upd |= ImGui::ColorEdit3("Background", (float*)&gColBackground, ImGuiColorEditFlags_NoInputs);
//        upd |= ImGui::ColorEdit3("Border", (float*)&gColBorder, ImGuiColorEditFlags_NoInputs);
//
//
//        if (upd)
//        {
//            ConstructThemeSimple(gColWidgets, gColSeparator, gColActive, gColBackground, gColBorder);
//            ImGui_ImplSwizzle_SetupStyle(gStyle);
//        }
//
//        int idx = 1;
//        for (const auto& cp : gStyle.mColors)
//        {
//            std::string id = "## widget_" + std::to_string(idx);
//            ImGui::ColorButton(id.c_str(), cp.second, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);
//            if (idx % 4 != 0)
//            {
//                ImGui::SameLine();
//            }
//            idx++;
//        }
//    }
//
//    ImGui::End();
//}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
