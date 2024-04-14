#include "ImGuiThemes.hpp"

#include <cstdint>

#include "imgui.h"
#include "implot.h"

auto SetGreenColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(20, 6);
    style.WindowTitleAlign = ImVec2(0.30f, 0.50f);
    style.ScrollbarSize = 17;
    style.FramePadding = ImVec2(5, 6);

    style.FrameRounding = 0;
    style.WindowRounding = 0;
    style.ScrollbarRounding = 0;
    style.ChildRounding = 0;
    style.PopupRounding = 0;
    style.GrabRounding = 0;
    style.TabRounding = 0;

    style.WindowBorderSize = 1;
    style.FrameBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.TabBorderSize = 1;

    style.Colors[ImGuiCol_Text]                   = ImVec4(0.85f, 0.87f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.63f, 0.67f, 0.58f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Border]                 = ImVec4(0.53f, 0.57f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.16f, 0.18f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.24f, 0.27f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.24f, 0.27f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.24f, 0.27f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.35f, 0.42f, 0.31f, 0.52f);
    style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                 = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.49f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_Separator]              = ImVec4(0.16f, 0.18f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.16f, 0.18f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.16f, 0.18f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Tab]                    = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TabActive]              = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.30f, 0.35f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]              = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.26f, 0.59f, 0.98f, 0.34f);
    style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 1.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
    style.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.80f, 0.80f, 0.80f, 0.31f);
    style.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.80f, 0.80f, 0.80f, 0.38f);
    style.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.58f, 0.53f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.58f, 0.53f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.12f, 0.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);
}

auto SetGrayColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(20, 6);
    style.WindowTitleAlign = ImVec2(0.30f, 0.50f);
    style.ScrollbarSize = 17;
    style.FramePadding = ImVec2(5, 6);

    style.FrameRounding = 0;
    style.WindowRounding = 0;
    style.ScrollbarRounding = 0;
    style.ChildRounding = 0;
    style.PopupRounding = 0;
    style.GrabRounding = 0;
    style.TabRounding = 0;

    style.WindowBorderSize = 1;
    style.FrameBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.TabBorderSize = 1;

    style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Border]                 = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.52f);
    style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 0.49f, 0.11f, 0.54f);
    style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.49f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.41f);
    style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(1.00f, 0.49f, 0.11f, 0.34f);
    style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.49f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Tab]                    = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabActive]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]              = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.26f, 0.59f, 0.98f, 0.34f);
    style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 1.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
    style.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.80f, 0.80f, 0.80f, 0.31f);
    style.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.80f, 0.80f, 0.80f, 0.38f);
    style.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.00f, 0.49f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 0.49f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.12f, 0.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);
}

auto SetDarkColorStyle() -> void {

    auto& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(3, 2);
    style.ItemSpacing = ImVec2(4, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 15.0f;
    style.GrabMinSize = 5.0f;

    style.WindowRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 2.0f;

    auto colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(19 / 255.0f, 25 / 255.0f, 28 / 255.0f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, .75f);    
}

auto SetDarkerColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(4.0f, 4.0f);
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.ScrollbarSize = 14.0f;
    style.IndentSpacing = 14.0f;
    style.GrabMinSize = 12.0f;
    style.FramePadding = ImVec2(8.0f, 4.0f);

    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowRounding = 4.0f;

    style.AntiAliasedFill = true;
    style.AntiAliasedLines = true;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.196f, 0.196f, 0.196f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.196f, 0.196f, 0.196f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.36f, 0.36f, 0.36f, 0.21f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.27f, 0.37f, 0.13f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.34f, 0.47f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.41f, 0.56f, 0.20f, 0.99f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.27f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.59f, 0.59f, 0.59f, 0.98f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.83f, 0.83f, 0.83f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.83f, 0.83f, 0.83f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.50f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.50f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
}

auto SetLightGrayColorStyle() -> void {
    
    auto& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(4.0f,2.0f);
    style.ItemSpacing = ImVec2(8.0f,2.0f);
    style.WindowRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 1.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.44f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.71f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.63f, 0.63f, 0.63f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.43f, 0.44f, 0.46f, 0.78f);
    colors[ImGuiCol_Button]                 = ImVec4(0.61f, 0.61f, 0.62f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.57f, 0.57f, 0.57f, 0.52f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.61f, 0.63f, 0.64f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.64f, 0.64f, 0.65f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.58f, 0.58f, 0.59f, 0.55f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.52f, 0.52f, 0.52f, 0.55f);
    colors[ImGuiCol_Separator]              = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.71f, 0.72f, 0.73f, 0.57f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.16f, 0.16f, 0.17f, 0.95f);  
    style.WindowMenuButtonPosition = ImGuiDir_None;    
}

auto SetLightColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
    style.Colors[ImGuiCol_ChildBg]                  = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// https://github.com/ocornut/imgui/issues/707#issuecomment-917151020
auto SetJanekbDarknessColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 10;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    style.WindowRounding                    = 7;
    style.ChildRounding                     = 4;
    style.FrameRounding                     = 3;
    style.PopupRounding                     = 4;
    style.ScrollbarRounding                 = 9;
    style.GrabRounding                      = 3;
    style.LogSliderDeadzone                 = 4;
    style.TabRounding                       = 4;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
}

ImVec4 from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool consistent_color = false)
{
    auto res = ImVec4(r / (float)255, g / (float)255, b / (float)255, a / (float)255);
    return res;
}

auto Color(unsigned int c) -> uint32_t {
    // add alpha.
    // also swap red and blue channel for some reason.
    // todo: figure out why, and fix it.
    const short a = 0xFF;
    const short r = (c >> 16) & 0xFF;
    const short g = (c >> 8) & 0xFF;
    const short b = (c >> 0) & 0xFF;
    return(a << 24)
        | (r << 0)
        | (g << 8)
        | (b << 16);
}

namespace SpectrumLight {
    const unsigned int NONE = 0x00000000; // transparent    
    const unsigned int GRAY50 = Color(0xFFFFFF);
    const unsigned int GRAY75 = Color(0xFAFAFA);
    const unsigned int GRAY100 = Color(0xF5F5F5);
    const unsigned int GRAY200 = Color(0xEAEAEA);
    const unsigned int GRAY300 = Color(0xE1E1E1);
    const unsigned int GRAY400 = Color(0xCACACA);
    const unsigned int GRAY500 = Color(0xB3B3B3);
    const unsigned int GRAY600 = Color(0x8E8E8E);
    const unsigned int GRAY700 = Color(0x707070);
    const unsigned int GRAY800 = Color(0x4B4B4B);
    const unsigned int GRAY900 = Color(0x2C2C2C);
    const unsigned int BLUE400 = Color(0x2680EB);
    const unsigned int BLUE500 = Color(0x1473E6);
    const unsigned int BLUE600 = Color(0x0D66D0);
    const unsigned int BLUE700 = Color(0x095ABA);
    const unsigned int RED400 = Color(0xE34850);
    const unsigned int RED500 = Color(0xD7373F);
    const unsigned int RED600 = Color(0xC9252D);
    const unsigned int RED700 = Color(0xBB121A);
    const unsigned int ORANGE400 = Color(0xE68619);
    const unsigned int ORANGE500 = Color(0xDA7B11);
    const unsigned int ORANGE600 = Color(0xCB6F10);
    const unsigned int ORANGE700 = Color(0xBD640D);
    const unsigned int GREEN400 = Color(0x2D9D78);
    const unsigned int GREEN500 = Color(0x268E6C);
    const unsigned int GREEN600 = Color(0x12805C);
    const unsigned int GREEN700 = Color(0x107154);
    const unsigned int INDIGO400 = Color(0x6767EC);
    const unsigned int INDIGO500 = Color(0x5C5CE0);
    const unsigned int INDIGO600 = Color(0x5151D3);
    const unsigned int INDIGO700 = Color(0x4646C6);
    const unsigned int CELERY400 = Color(0x44B556);
    const unsigned int CELERY500 = Color(0x3DA74E);
    const unsigned int CELERY600 = Color(0x379947);
    const unsigned int CELERY700 = Color(0x318B40);
    const unsigned int MAGENTA400 = Color(0xD83790);
    const unsigned int MAGENTA500 = Color(0xCE2783);
    const unsigned int MAGENTA600 = Color(0xBC1C74);
    const unsigned int MAGENTA700 = Color(0xAE0E66);
    const unsigned int YELLOW400 = Color(0xDFBF00);
    const unsigned int YELLOW500 = Color(0xD2B200);
    const unsigned int YELLOW600 = Color(0xC4A600);
    const unsigned int YELLOW700 = Color(0xB79900);
    const unsigned int FUCHSIA400 = Color(0xC038CC);
    const unsigned int FUCHSIA500 = Color(0xB130BD);
    const unsigned int FUCHSIA600 = Color(0xA228AD);
    const unsigned int FUCHSIA700 = Color(0x93219E);
    const unsigned int SEAFOAM400 = Color(0x1B959A);
    const unsigned int SEAFOAM500 = Color(0x16878C);
    const unsigned int SEAFOAM600 = Color(0x0F797D);
    const unsigned int SEAFOAM700 = Color(0x096C6F);
    const unsigned int CHARTREUSE400 = Color(0x85D044);
    const unsigned int CHARTREUSE500 = Color(0x7CC33F);
    const unsigned int CHARTREUSE600 = Color(0x73B53A);
    const unsigned int CHARTREUSE700 = Color(0x6AA834);
    const unsigned int PURPLE400 = Color(0x9256D9);
    const unsigned int PURPLE500 = Color(0x864CCC);
    const unsigned int PURPLE600 = Color(0x7A42BF);
    const unsigned int PURPLE700 = Color(0x6F38B1);
}

namespace SpectrumDark {
    const unsigned int NONE = 0x00000000; // transparent
    const unsigned int GRAY50 = Color(0x252525);
    const unsigned int GRAY75 = Color(0x2F2F2F);
    const unsigned int GRAY100 = Color(0x323232);
    const unsigned int GRAY200 = Color(0x393939);
    const unsigned int GRAY300 = Color(0x3E3E3E);
    const unsigned int GRAY400 = Color(0x4D4D4D);
    const unsigned int GRAY500 = Color(0x5C5C5C);
    const unsigned int GRAY600 = Color(0x7B7B7B);
    const unsigned int GRAY700 = Color(0x999999);
    const unsigned int GRAY800 = Color(0xCDCDCD);
    const unsigned int GRAY900 = Color(0xFFFFFF);
    const unsigned int BLUE400 = Color(0x2680EB);
    const unsigned int BLUE500 = Color(0x378EF0);
    const unsigned int BLUE600 = Color(0x4B9CF5);
    const unsigned int BLUE700 = Color(0x5AA9FA);
    const unsigned int RED400 = Color(0xE34850);
    const unsigned int RED500 = Color(0xEC5B62);
    const unsigned int RED600 = Color(0xF76D74);
    const unsigned int RED700 = Color(0xFF7B82);
    const unsigned int ORANGE400 = Color(0xE68619);
    const unsigned int ORANGE500 = Color(0xF29423);
    const unsigned int ORANGE600 = Color(0xF9A43F);
    const unsigned int ORANGE700 = Color(0xFFB55B);
    const unsigned int GREEN400 = Color(0x2D9D78);
    const unsigned int GREEN500 = Color(0x33AB84);
    const unsigned int GREEN600 = Color(0x39B990);
    const unsigned int GREEN700 = Color(0x3FC89C);
    const unsigned int INDIGO400 = Color(0x6767EC);
    const unsigned int INDIGO500 = Color(0x7575F1);
    const unsigned int INDIGO600 = Color(0x8282F6);
    const unsigned int INDIGO700 = Color(0x9090FA);
    const unsigned int CELERY400 = Color(0x44B556);
    const unsigned int CELERY500 = Color(0x4BC35F);
    const unsigned int CELERY600 = Color(0x51D267);
    const unsigned int CELERY700 = Color(0x58E06F);
    const unsigned int MAGENTA400 = Color(0xD83790);
    const unsigned int MAGENTA500 = Color(0xE2499D);
    const unsigned int MAGENTA600 = Color(0xEC5AAA);
    const unsigned int MAGENTA700 = Color(0xF56BB7);
    const unsigned int YELLOW400 = Color(0xDFBF00);
    const unsigned int YELLOW500 = Color(0xEDCC00);
    const unsigned int YELLOW600 = Color(0xFAD900);
    const unsigned int YELLOW700 = Color(0xFFE22E);
    const unsigned int FUCHSIA400 = Color(0xC038CC);
    const unsigned int FUCHSIA500 = Color(0xCF3EDC);
    const unsigned int FUCHSIA600 = Color(0xD951E5);
    const unsigned int FUCHSIA700 = Color(0xE366EF);
    const unsigned int SEAFOAM400 = Color(0x1B959A);
    const unsigned int SEAFOAM500 = Color(0x20A3A8);
    const unsigned int SEAFOAM600 = Color(0x23B2B8);
    const unsigned int SEAFOAM700 = Color(0x26C0C7);
    const unsigned int CHARTREUSE400 = Color(0x85D044);
    const unsigned int CHARTREUSE500 = Color(0x8EDE49);
    const unsigned int CHARTREUSE600 = Color(0x9BEC54);
    const unsigned int CHARTREUSE700 = Color(0xA3F858);
    const unsigned int PURPLE400 = Color(0x9256D9);
    const unsigned int PURPLE500 = Color(0x9D64E1);
    const unsigned int PURPLE600 = Color(0xA873E9);
    const unsigned int PURPLE700 = Color(0xB483F0);
}

// https://github.com/adobe/imgui/blob/master/imgui_spectrum.h
auto SetAdobeSpectrumDarkColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.GrabRounding = 4.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY800); // text on hovered controls is gray900
    colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY500);
    colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY100);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY50); // not sure about this. Note: applies to tooltips too.
    colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY300);
    colors[ImGuiCol_BorderShadow] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::NONE); // We don't want shadows. Ever.
    colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY75); // this isnt right, spectrum does not do this, but it's a good fallback
    colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY50);
    colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY200);
    colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY300); // those titlebar values are totally made up, spectrum does not have this.
    colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY200);
    colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY400);
    colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY100);
    colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY100); // same as regular background
    colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY400);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY600);
    colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY700);
    colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE500);
    colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY700);
    colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY800);
    colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY75); // match default button to Spectrum's 'Action Button'.
    colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY50);
    colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY200);
    colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE400);
    colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE500);
    colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE600);
    colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY400);
    colors[ImGuiCol_SeparatorHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY600);
    colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY700);
    colors[ImGuiCol_ResizeGrip] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY400);
    colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY600);
    colors[ImGuiCol_ResizeGripActive] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::GRAY700);
    colors[ImGuiCol_PlotLines] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE400);
    colors[ImGuiCol_PlotLinesHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE600);
    colors[ImGuiCol_PlotHistogram] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE400);
    colors[ImGuiCol_PlotHistogramHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumDark::BLUE600);
    colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4((SpectrumDark::BLUE400 & 0x00FFFFFF) | 0x33000000);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImGui::ColorConvertU32ToFloat4((SpectrumDark::GRAY900 & 0x00FFFFFF) | 0x0A000000);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

auto SetAdobeSpectrumLightColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.GrabRounding = 4.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY800); // text on hovered controls is gray900
    colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY500);
    colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY100);
    colors[ImGuiCol_ChildBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY100);
    colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY50); // not sure about this. Note: applies to tooltips too.
    colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY300);
    colors[ImGuiCol_BorderShadow] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::NONE); // We don't want shadows. Ever.
    colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY75); // this isnt right, spectrum does not do this, but it's a good fallback
    colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY50);
    colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY200);
    colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY300); // those titlebar values are totally made up, spectrum does not have this.
    colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY200);
    colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY400);
    colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY100);
    colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY100); // same as regular background
    colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY400);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY600);
    colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY700);
    colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE500);
    colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY700);
    colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY800);
    colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY75); // match default button to Spectrum's 'Action Button'.
    colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY50);
    colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY200);
    colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE400);
    colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE500);
    colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE600);
    colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY400);
    colors[ImGuiCol_SeparatorHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY600);
    colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY700);
    colors[ImGuiCol_ResizeGrip] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY400);
    colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY600);
    colors[ImGuiCol_ResizeGripActive] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY700);
    colors[ImGuiCol_PlotLines] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE400);
    colors[ImGuiCol_PlotLinesHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE600);
    colors[ImGuiCol_PlotHistogram] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE400);
    colors[ImGuiCol_PlotHistogramHovered] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::BLUE600);
    colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4((SpectrumLight::BLUE400 & 0x00FFFFFF) | 0x33000000);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImGui::ColorConvertU32ToFloat4((SpectrumLight::GRAY900 & 0x00FFFFFF) | 0x0A000000);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImGui::ColorConvertU32ToFloat4(SpectrumLight::GRAY100);
}

// https://github.com/IntelRealSense/librealsense/blob/master/tools/rosbag-inspector/rs-rosbag-inspector.cpp
auto SetIntelRealSenseColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;

    static const ImVec4 light_blue = from_rgba(0, 174, 239, 255, true); // Light blue color for selected elements such as play button glyph when paused
    static const ImVec4 regular_blue = from_rgba(0, 115, 200, 255, true); // Checkbox mark, slider grabber
    static const ImVec4 light_grey = from_rgba(0xc3, 0xd5, 0xe5, 0xff, true); // Text
    static const ImVec4 dark_window_background = from_rgba(9, 11, 13, 255);
    static const ImVec4 almost_white_bg = from_rgba(230, 230, 230, 255, true);
    static const ImVec4 black = from_rgba(0, 0, 0, 255, true);
    static const ImVec4 transparent = from_rgba(0, 0, 0, 0, true);
    static const ImVec4 white = from_rgba(0xff, 0xff, 0xff, 0xff, true);
    static const ImVec4 scrollbar_bg = from_rgba(14, 17, 20, 255);
    static const ImVec4 scrollbar_grab = from_rgba(54, 66, 67, 255);
    static const ImVec4 scrollbar_grabp1 = from_rgba(64, 76, 77, 255);
    static const ImVec4 scrollbar_grabm1 = from_rgba(44, 56, 57, 255);    
    static const ImVec4 grey{ 0.5f,0.5f,0.5f,1.f };
    static const ImVec4 dark_grey = from_rgba(30, 30, 30, 255);
    static const ImVec4 sensor_header_light_blue = from_rgba(80, 99, 115, 0xff);
    static const ImVec4 sensor_bg = from_rgba(36, 44, 51, 0xff);
    static const ImVec4 redish = from_rgba(255, 46, 54, 255, true);
    static const ImVec4 light_red = from_rgba(255, 146, 154, 255, true);
    static const ImVec4 dark_red = from_rgba(200, 46, 54, 255, true);
    static const ImVec4 button_color = from_rgba(62, 77, 89, 0xff);
    static const ImVec4 button_colorp1 = from_rgba(72, 87, 99, 0xff);
    static const ImVec4 button_colorm1 = from_rgba(52, 67, 79, 0xff);
    static const ImVec4 header_window_bg = from_rgba(36, 44, 54, 0xff);
    static const ImVec4 header_color = from_rgba(62, 77, 89, 255);
    static const ImVec4 header_colorp1 = from_rgba(72, 87, 99, 255);
    static const ImVec4 header_colorm1 = from_rgba(52, 67, 79, 255);    
    static const ImVec4 title_color = from_rgba(27, 33, 38, 255);
    static const ImVec4 device_info_color = from_rgba(33, 40, 46, 255);
    static const ImVec4 yellow = from_rgba(229, 195, 101, 255, true);
    static const ImVec4 yellowish = from_rgba(255, 253, 191, 255, true);
    static const ImVec4 green = from_rgba(0x20, 0xe0, 0x20, 0xff, true);
    static const ImVec4 dark_sensor_bg = from_rgba(0x1b, 0x21, 0x25, 170);
    static const ImVec4 red = from_rgba(233, 0, 0, 255, true);
    static const ImVec4 greenish = from_rgba(67, 163, 97, 255);
    static const ImVec4 orange = from_rgba(255, 157, 0, 255, true);    

    style.Colors[ImGuiCol_WindowBg] = dark_window_background;
    style.Colors[ImGuiCol_Border] = black;
    style.Colors[ImGuiCol_BorderShadow] = transparent;
    style.Colors[ImGuiCol_FrameBg] = dark_window_background;
    style.Colors[ImGuiCol_ScrollbarBg] = scrollbar_bg;
    style.Colors[ImGuiCol_ScrollbarGrab] = scrollbar_grab;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = scrollbar_grab;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = scrollbar_grab;
    style.Colors[ImGuiCol_CheckMark] = regular_blue;
    style.Colors[ImGuiCol_SliderGrab] = regular_blue;
    style.Colors[ImGuiCol_SliderGrabActive] = regular_blue;
    style.Colors[ImGuiCol_Button] = button_color;
    style.Colors[ImGuiCol_ButtonHovered] = button_colorp1;
    style.Colors[ImGuiCol_ButtonActive] = button_colorm1;
    style.Colors[ImGuiCol_Header] = header_color;
    style.Colors[ImGuiCol_HeaderActive] = header_colorm1;
    style.Colors[ImGuiCol_HeaderHovered] = header_colorp1;
    style.Colors[ImGuiCol_TitleBg] = title_color;
    style.Colors[ImGuiCol_TitleBgCollapsed] = title_color;
    style.Colors[ImGuiCol_TitleBgActive] = header_color;
}

// https://github.com/ocornut/imgui/issues/707#issuecomment-1372640066
auto SetDraculaColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;

    auto& colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Border
    colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

    // Text
    colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
    colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
    colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

    // Docking
    colors[ImGuiCol_DockingPreview] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
}

// https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
auto SetBrightWhiteColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]           = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// https://github.com/ocornut/imgui/issues/1902#issuecomment-408594028
auto SetPurpleDertsehaColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    auto colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.13f, 1.00f, 0.26f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.02f, 0.80f, 0.58f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.19f, 0.00f, 0.22f, 0.80f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.00f, 0.22f, 0.75f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.60f, 0.35f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.32f, 0.60f, 0.35f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.02f, 0.80f, 0.58f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.02f, 0.80f, 0.58f, 0.67f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.02f, 0.80f, 0.58f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.02f, 0.80f, 0.58f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.02f, 0.80f, 0.58f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.02f, 0.80f, 0.58f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.32f, 0.60f, 0.35f, 0.70f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.02f, 0.80f, 0.58f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.02f, 0.80f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.02f, 0.80f, 0.58f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.02f, 0.80f, 0.58f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.02f, 0.80f, 0.58f, 0.95f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.02f, 0.80f, 0.58f, 0.35f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.77f, 0.22f, 0.62f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.77f, 0.22f, 0.62f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.77f, 0.22f, 0.62f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.32f, 0.60f, 0.35f, 1.00f);
}

auto SetInochiDarkColorStyle() -> void {

    auto& style = ImGui::GetStyle();
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;

    style.WindowRounding = 4;
    style.ChildRounding = 0;
    style.FrameRounding = 3;
    style.PopupRounding = 6;
    style.ScrollbarRounding = 18;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 6;
    style.TabRounding = 6;

    style.IndentSpacing = 10;
    style.ItemSpacing.y = 3;
    style.FramePadding.y = 4;

    style.GrabMinSize = 13;
    style.ScrollbarSize = 14;
    style.ChildBorderSize = 1;

    style.WindowMenuButtonPosition = ImGuiDir_None;
    
    style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.16f);
    style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.16f);
    style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.15f, 0.15f, 0.15f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_Button]                 = ImVec4(0.39f, 0.39f, 0.39f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.28f, 0.28f, 0.28f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_Separator]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.29f, 0.29f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.35f, 0.35f, 0.35f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.40f, 0.40f, 0.40f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.55f, 0.55f, 0.56f, 0.00f);
    style.Colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.34f, 0.34f, 0.34f, 0.80f);
    style.Colors[ImGuiCol_TabActive]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.14f, 0.14f, 0.14f, 0.97f);
    style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.62f, 0.68f, 0.75f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.310f, 0.310f, 0.310f, 0.267f);
    style.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.463f, 0.463f, 0.463f, 0.267f);
    style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight]           = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);    
}

auto SetHedgeGIColorStyle() -> void {

    auto& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(4.0f, 4.0f);
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.ScrollbarSize = 14.0f;
    style.IndentSpacing = 14.0f;
    style.GrabMinSize = 12.0f;
    style.FramePadding = ImVec2(8.0f, 4.0f);

    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowRounding = 4.0f;

    style.AntiAliasedFill = true;
    style.AntiAliasedLines = true;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.196f, 0.196f, 0.196f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.196f, 0.196f, 0.196f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.36f, 0.36f, 0.36f, 0.21f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.27f, 0.37f, 0.13f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.34f, 0.47f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.41f, 0.56f, 0.20f, 0.99f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.27f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.59f, 0.59f, 0.59f, 0.98f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.83f, 0.83f, 0.83f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.83f, 0.83f, 0.83f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.50f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.50f);    
}

// https://github.com/epezent/etfe/blob/main/examples/filter_toy.cpp#L242
auto SetEtfeOrangeColorStyle() -> void {

    // green
    // ImVec4 dark_accent  = ImVec4(0.00f, 0.70f, 0.16f, 1.00f); 
    // ImVec4 light_accent = ImVec4(0.50f, 1.00f, 0.00f, 1.00f);

    // orange
    ImVec4 dark_accent  = ImVec4(0.85f, 0.37f, 0.00f, 1.00f);
    ImVec4 light_accent = ImVec4(1.00f, 0.63f, 0.00f, 1.00f);

    // purple
    // ImVec4 dark_accent = ImVec4(0.416f, 0.000f, 1.000f, 1.000f);
    // ImVec4 light_accent = ImVec4(0.691f, 0.484f, 0.973f, 1.000f);

    auto& style = ImGui::GetStyle();
    style.WindowPadding = {6,6};
    style.FramePadding  = {6,3};
    style.CellPadding   = {6,3};
    style.ItemSpacing   = {6,6};
    style.ItemInnerSpacing = {6,6};
    style.ScrollbarSize = 16;
    style.GrabMinSize = 8;
    style.WindowBorderSize = style.ChildBorderSize = style.PopupBorderSize = style.TabBorderSize = 0;
    style.FrameBorderSize = 1;
    style.WindowRounding = style.ChildRounding = style.PopupRounding = style.ScrollbarRounding = style.GrabRounding = style.TabRounding = 4;


    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(0.89f, 0.89f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.20f, 0.21f, 0.27f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.20f, 0.21f, 0.27f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.173f, 0.184f, 0.235f, 1.000f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.06f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
    colors[ImGuiCol_FrameBgHovered]         = light_accent;
    colors[ImGuiCol_FrameBgActive]          = light_accent;
    colors[ImGuiCol_TitleBg]                = dark_accent;
    colors[ImGuiCol_TitleBgActive]          = dark_accent;
    colors[ImGuiCol_TitleBgCollapsed]       = dark_accent;
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.21f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.89f, 0.89f, 0.93f, 0.27f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.89f, 0.89f, 0.93f, 0.55f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_CheckMark]              = dark_accent;
    colors[ImGuiCol_SliderGrab]             = dark_accent;
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Button]                 = dark_accent;
    colors[ImGuiCol_ButtonHovered]          = light_accent;
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_Header]                 = dark_accent;
    colors[ImGuiCol_HeaderHovered]          = light_accent;
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_Separator]              = dark_accent;
    colors[ImGuiCol_SeparatorHovered]       = light_accent;
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = dark_accent;
    colors[ImGuiCol_ResizeGripHovered]      = light_accent;
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
    colors[ImGuiCol_TabHovered]             = light_accent;
    colors[ImGuiCol_TabActive]              = dark_accent;
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_PlotLines]              = light_accent;
    colors[ImGuiCol_PlotLinesHovered]       = light_accent;
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

    ImVec4* pcolors = ImPlot::GetStyle().Colors;
    pcolors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_FrameBg]       = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_PlotBg]        = ImVec4(0.07f, 0.07f, 0.10f, 0.00f);
    pcolors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    pcolors[ImPlotCol_LegendBg]      = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_LegendBorder]  = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_LegendText]    = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_TitleText]     = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_InlayText]     = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_AxisGrid]         = IMPLOT_AUTO_COL;
    pcolors[ImPlotCol_Selection]     = ImVec4(0.000f, 0.571f, 1.000f, 1.000f);
    pcolors[ImPlotCol_Crosshairs]    = IMPLOT_AUTO_COL;

    auto& pstyle = ImPlot::GetStyle();
    // pstyle.AntiAliasedLines = true;
    pstyle.PlotPadding = pstyle.LegendPadding = {12,12};
    pstyle.LabelPadding = pstyle.LegendInnerPadding = {6,6};
    pstyle.FitPadding   = {0,0.1f};
    pstyle.LegendSpacing = {2,2};
    pstyle.AnnotationPadding = {6,4};

    static const ImVec4 colormap[2] = {
        dark_accent,
        light_accent
    };

    //ImPlot::SetColormap(colormap,2);
}