
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include <swizzle/core/Platform.hpp>

#include "ImGuiSwizzlePlf.hpp"
#include "ImGuiTranslateKey.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

class InputCallback : public swizzle::EventHandler<swizzle::core::WindowEvent>
{
protected:
    virtual void publishEvent(const swizzle::core::WindowEvent& evt) override;
};

struct ImGui_ImplSwizzle_PlfData
{
    common::Resource<swizzle::core::SwWindow> mMainWindow;
    bool mWantUpdateMonitors;
    InputCallback mInput;
};


/* Static Variables */

/* Static Function Declaration */

static ImGui_ImplSwizzle_PlfData* ImGui_ImplSwizzle_GetPlfBackendData();

static void ImGui_ImplSwizzle_UpdateMonitors();
static void ImGui_ImplSwizzle_UpdateMouseData();

/* Static Function Definition */

static ImGui_ImplSwizzle_PlfData* ImGui_ImplSwizzle_GetPlfBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplSwizzle_PlfData*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

static void ImGui_ImplSwizzle_UpdateMonitors()
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    bd->mWantUpdateMonitors = false;

    platform_io.Monitors.resize(0);

    U32 monitorCount = swizzle::core::GetMonitorCount();
    for (U32 n = 0; n < monitorCount; n++)
    {
        ImGuiPlatformMonitor monitor{};
        swizzle::core::MonitorInfo mon = swizzle::core::GetMonitorInfo(n);

        monitor.WorkPos = ImVec2{ static_cast<float>(mon.mXPos), static_cast<float>(mon.mYPos) };
        monitor.MainSize = ImVec2{ static_cast<float>(mon.mWidth), static_cast<float>(mon.mHeight) };

        platform_io.Monitors.push_back(monitor);
    }
}

static void ImGui_ImplSwizzle_UpdateMouseData()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

    ImGuiID mouseViewportId = 0;
    const ImVec2 mousePrevPos = io.MousePos;
    for (int n = 0; n < platformIO.Viewports.Size; n++)
    {
        ImGuiViewport* viewport = platformIO.Viewports[n];
        swizzle::core::SwWindow* wnd = (swizzle::core::SwWindow*)viewport->PlatformHandle;

        if (wnd->hasFocus())
        {
            if (io.WantSetMousePos)
            {
                // wnd->setCursorPos();
            }

            U32 mouseX, mouseY;
            wnd->getCursorPos(mouseX, mouseY);
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                U32 winX, winY;
                wnd->getWindowPos(winX, winY);
                mouseX += winX;
                mouseY += winY;
            }

            io.AddMousePosEvent(static_cast<float>(mouseX), static_cast<float>(mouseY));
        }
    }

    if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)
        io.AddMouseViewportEvent(mouseViewportId);
}

/* Function Definition */

bool ImGui_ImplSwizzle_Plf_Init(common::Resource<swizzle::core::SwWindow> window)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    ImGui_ImplSwizzle_PlfData* bd = IM_NEW(ImGui_ImplSwizzle_PlfData)();
    bd->mMainWindow = window;
    bd->mMainWindow->addEventListener(&bd->mInput);

    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_swizzle_platform";
    //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    // This bypasses the smart pointer, and is a potential use after free
    mainViewport->PlatformHandle = (void*)bd->mMainWindow.get();

    // Update monitors
    bd->mWantUpdateMonitors = true;
    ImGui_ImplSwizzle_UpdateMonitors();

    // TODO: init platform interfaces
    if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)
    {
        //ImGui_ImplSwizzle_Plf_InitInterface();
    }

    return true;
}

void ImGui_ImplSwizzle_Plf_Shutdown()
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    // TODO: shutdown platform interfaces
    //ImGui_ImplSwizzle_Plf_ShudownInterface();

    bd->mMainWindow->removeEventListener(&bd->mInput);

    // cleanup main viewport, // io.BackendPlatformUserData still contains handle to main window
    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    mainViewport->PlatformHandle = nullptr;

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;

    io.BackendFlags &= ImGuiBackendFlags_PlatformHasViewports;
    IM_DELETE(bd);
}

void ImGui_ImplSwizzle_Plf_NewFrame(DeltaTime dt)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = dt;

    U32 w, h;
    bd->mMainWindow->getSize(w, h);
    io.DisplaySize = ImVec2((float)w, (float)h);

    if (bd->mWantUpdateMonitors)
    {
        ImGui_ImplSwizzle_UpdateMonitors();
    }

    // Update OS mouse position
    ImGui_ImplSwizzle_UpdateMouseData();
}

common::Resource<swizzle::core::SwWindow> ImGui_ImplSwizzle_Plf_GetViewportWindow(ImGuiViewport* viewport)
{
    UNUSED_ARG(viewport);
    return nullptr;
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

void InputCallback::publishEvent(const swizzle::core::WindowEvent& evt)
{
    SWIZZLE_PROFILE_EVENT("ImGuiInputCallback::publishEvent");
    ImGuiIO& io = ImGui::GetIO();

    auto evtType = evt.getEventType();
    switch (evtType)
    {
    case swizzle::core::WindowEventType::CharacterTypeEvent:
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
        }
        else
        {
            io.AddMouseButtonEvent(e.mKey - 1, e.mPressed);
        }
        break;
    }
    case swizzle::core::WindowEventType::FocusEvent:
    {
        swizzle::core::WindowFocusEvent& e = (swizzle::core::WindowFocusEvent&)evt;
        io.AddFocusEvent(e.mFocused);
        break;
    }
    case swizzle::core::WindowEventType::MouseMoveEvent:
    {
        swizzle::core::MouseMoveEvent& e = (swizzle::core::MouseMoveEvent&)evt;

        U32 x = e.mX;
        U32 y = e.mY;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            U32 winX, winY;
            e.mWindow->getWindowPos(winX, winY);
            x += winX;
            y += winY;
        }
        io.AddMousePosEvent((float)x, (float)y);

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
        break;
    }
    default:
        break;
    }
}

/* Class Private Function Definition */
