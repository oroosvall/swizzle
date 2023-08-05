
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include <swizzle/core/Platform.hpp>

#include "ImGuiSwizzlePlf.hpp"
#include "ImGuiTranslateKey.hpp"

/* Defines */

/* Typedefs */

using Window = swizzle::core::SwWindow;

/* Structs/Classes */

class MainInputCallback : public swizzle::EventHandler<swizzle::core::WindowEvent>
{
protected:
    virtual void publishEvent(const swizzle::core::WindowEvent& evt) override;
};

class ViewportInputCallback : public swizzle::EventHandler<swizzle::core::WindowEvent>
{
protected:
    virtual void publishEvent(const swizzle::core::WindowEvent& evt) override;
};

struct ImGui_ImplSwizzle_PlfViewportData
{
    common::Resource<Window> mWindow;
};

struct ImGui_ImplSwizzle_PlfData
{
    common::Resource<Window> mMainWindow = nullptr;
    bool mWantUpdateMonitors = false;
    MainInputCallback mInput;
    ViewportInputCallback mViewportInput;
    ImVec2 mLastValidMousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    Window* mMouseWindow = nullptr;

    Window* mKeyOwnerWindow[ImGuiKey::ImGuiKey_COUNT];
    Window* mMouseButtonOwnerWindow[ImGuiMouseButton_::ImGuiMouseButton_COUNT];
};

/* Static Variables */

/* Static Function Declaration */

static ImGui_ImplSwizzle_PlfData* ImGui_ImplSwizzle_GetPlfBackendData();

static void ImGui_ImplSwizzle_UpdateMonitors();
static void ImGui_ImplSwizzle_UpdateMouseData();

static void ImGui_ImplSwizzle_FocusCallback(Window* window, bool focus);
static void ImGui_ImplSwizzle_CursorEnterCallback(Window* window, bool enter);
static void ImGui_ImplSwizzle_CursorPosCallback(Window* window, S32 xPos, S32 yPos);
static void ImGui_ImplSwizzle_ScrollCallback(Window* window, S32 xPos, S32 yPos);
static void ImGui_ImplSwizzle_KeyEventCallback(Window* window, ImGuiKey key, bool pressed);
static void ImGui_ImplSwizzle_MouseButtonCallback(Window* window, ImGuiMouseButton button, bool pressed);
static void ImGui_ImplSwizzle_CharCallback(Window* window, unsigned int ch);

static void ImGui_ImplSwizzle_InitPlatformIfc();
static void ImGui_ImplSwizzle_ShutdownPlatformIfc();

// Platform interface (multi-viewport callbacks)

static void ImGui_ImplSwizzle_CreateWindow(ImGuiViewport* viewport);
static void ImGui_ImplSwizzle_DestroyWindow(ImGuiViewport* viewport);
static void ImGui_ImplSwizzle_ShowWindow(ImGuiViewport* viewport);
static ImVec2 ImGui_ImplSwizzle_GetWindowPos(ImGuiViewport* viewport);
static void ImGui_ImplSwizzle_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
static ImVec2 ImGui_ImplSwizzle_GetWindowSize(ImGuiViewport* viewport);
static void ImGui_ImplSwizzle_SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
static void ImGui_ImplSwizzle_SetWindowTitle(ImGuiViewport* viewport, const char* title);
static bool ImGui_ImplSwizzle_GetWindowFocus(ImGuiViewport* viewport);


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

        monitor.WorkPos = ImVec2{ static_cast<float>(mon.mXPos), static_cast<float>(mon.mYPos) };
        monitor.WorkSize = ImVec2{ static_cast<float>(mon.mWidth), static_cast<float>(mon.mHeight) };

        platform_io.Monitors.push_back(monitor);
    }
}

static void ImGui_ImplSwizzle_UpdateMouseData()
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

    ImGuiID mouseViewportId = 0;
    const ImVec2 mousePrevPos = io.MousePos;
    for (int n = 0; n < platformIO.Viewports.Size; n++)
    {
        ImGuiViewport* viewport = platformIO.Viewports[n];
        Window* wnd = (Window*)viewport->PlatformHandle;

        if (wnd->hasFocus())
        {
            if (io.WantSetMousePos)
            {
                wnd->setCursorPos(
                    static_cast<S32>(mousePrevPos.x -viewport->Pos.x),
                    static_cast<S32>(mousePrevPos.y -viewport->Pos.y));
            }

            if(bd->mMouseWindow == nullptr)
            {
                S32 mouseX, mouseY;
                wnd->getCursorPos(mouseX, mouseY);
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    S32 winX, winY;
                    wnd->getWindowPos(winX, winY);
                    mouseX += winX;
                    mouseY += winY;
                }

                bd->mLastValidMousePos = ImVec2((float)mouseX, (float)mouseY);
                io.AddMousePosEvent(static_cast<float>(mouseX), static_cast<float>(mouseY));
            }
        }
    }

    if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)
        io.AddMouseViewportEvent(mouseViewportId);
}

static void ImGui_ImplSwizzle_FocusCallback(Window* window, bool focused)
{
    UNUSED_ARG(window);
    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

static void ImGui_ImplSwizzle_CursorEnterCallback(Window* window, bool entered)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();

    ImGuiIO& io = ImGui::GetIO();
    if(entered)
    {
        bd->mMouseWindow = window;
        io.AddMousePosEvent(bd->mLastValidMousePos.x, bd->mLastValidMousePos.y);
    }
    else if (!entered && bd->mMouseWindow == window)
    {
        bd->mLastValidMousePos = io.MousePos;
        bd->mMouseWindow = nullptr;
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }
}

static void ImGui_ImplSwizzle_CursorPosCallback(Window* window, S32 xPos, S32 yPos)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        S32 winX, winY;
        window->getWindowPos(winX, winY);
        xPos += winX;
        yPos += winY;
    }
    io.AddMousePosEvent((float)xPos, (float)yPos);
    bd->mLastValidMousePos = ImVec2((float)xPos, (float)yPos);
}

static void ImGui_ImplSwizzle_ScrollCallback(Window* window, S32 xPos, S32 yPos)
{
    UNUSED_ARG(window);
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent((float)xPos, (float)yPos);
}

static void ImGui_ImplSwizzle_KeyEventCallback(Window* window, ImGuiKey key, bool pressed)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiIO& io = ImGui::GetIO();

    bd->mKeyOwnerWindow[key] = window;

    io.AddKeyEvent(key, pressed);
}

static void ImGui_ImplSwizzle_MouseButtonCallback(Window* window, ImGuiMouseButton button, bool pressed)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiIO& io = ImGui::GetIO();

    bd->mMouseButtonOwnerWindow[button] = window;

    io.AddMouseButtonEvent(button, pressed);
}

static void ImGui_ImplSwizzle_CharCallback(Window* window, unsigned int ch)
{
    UNUSED_ARG(window);
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(ch);
}

static void ImGui_ImplSwizzle_InitPlatformIfc()
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
    platformIO.Platform_CreateWindow = &ImGui_ImplSwizzle_CreateWindow;
    platformIO.Platform_DestroyWindow = &ImGui_ImplSwizzle_DestroyWindow;
    platformIO.Platform_ShowWindow = &ImGui_ImplSwizzle_ShowWindow;
    platformIO.Platform_GetWindowPos = &ImGui_ImplSwizzle_GetWindowPos;
    platformIO.Platform_SetWindowPos = &ImGui_ImplSwizzle_SetWindowPos;
    platformIO.Platform_GetWindowSize = &ImGui_ImplSwizzle_GetWindowSize;
    platformIO.Platform_SetWindowSize = &ImGui_ImplSwizzle_SetWindowSize;
    platformIO.Platform_SetWindowTitle = &ImGui_ImplSwizzle_SetWindowTitle;
    platformIO.Platform_GetWindowFocus = &ImGui_ImplSwizzle_GetWindowFocus;

    // TODO: SetFocus?
    // TODO: GetWindowMinimized

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImGui_ImplSwizzle_PlfViewportData* vd = IM_NEW(ImGui_ImplSwizzle_PlfViewportData)();

    vd->mWindow = bd->mMainWindow;

    mainViewport->PlatformUserData = vd;
    mainViewport->PlatformHandle = bd->mMainWindow.get();

}

static void ImGui_ImplSwizzle_ShutdownPlatformIfc()
{
    ImGui::DestroyPlatformWindows();
}

static void ImGui_ImplSwizzle_CreateWindow(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    ImGui_ImplSwizzle_PlfViewportData* vd = IM_NEW(ImGui_ImplSwizzle_PlfViewportData)();
    viewport->PlatformUserData = vd;

    const U32 width = static_cast<U32>(viewport->Size.x);
    const U32 height = static_cast<U32>(viewport->Size.y);

    const S32 xPos = static_cast<S32>(viewport->Pos.x);
    const S32 yPos = static_cast<S32>(viewport->Pos.y);

    vd->mWindow = swizzle::core::CreateSwWindow(width, height, "No Title Yet!");
    vd->mWindow->setWindowPos(xPos, yPos);
    viewport->PlatformHandle = (void*)vd->mWindow.get();

    vd->mWindow->addEventListener(&bd->mViewportInput);
}

static void ImGui_ImplSwizzle_DestroyWindow(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    if(ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData)
    {

        for (int i = 0; i < IM_ARRAYSIZE(bd->mKeyOwnerWindow); i++)
        {
            if (bd->mKeyOwnerWindow[i] == vd->mWindow.get())
            {
                ImGui_ImplSwizzle_KeyEventCallback(vd->mWindow.get(), static_cast<ImGuiKey>(i), false);
            }
        }
        for (int i = 0; i < IM_ARRAYSIZE(bd->mMouseButtonOwnerWindow); i++)
        {
            if (bd->mMouseButtonOwnerWindow[i] == vd->mWindow.get())
            {
                ImGui_ImplSwizzle_MouseButtonCallback(vd->mWindow.get(), i, false);
            }
        }


        viewport->PlatformHandle = nullptr;
        vd->mWindow->removeEventListener(&bd->mViewportInput);
        vd->mWindow.reset();

        IM_DELETE(vd);
    }
    viewport->PlatformUserData = nullptr;
}

static void ImGui_ImplSwizzle_ShowWindow(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    vd->mWindow->show();
}

static ImVec2 ImGui_ImplSwizzle_GetWindowPos(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    S32 x = 0, y = 0;
    vd->mWindow->getWindowPos(x, y);
    return ImVec2(static_cast<float>(x), static_cast<float>(y));
}

static void ImGui_ImplSwizzle_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    vd->mWindow->setWindowPos(static_cast<S32>(pos.x), static_cast<S32>(pos.y));
}

static ImVec2 ImGui_ImplSwizzle_GetWindowSize(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    U32 x = 0, y = 0;
    vd->mWindow->getSize(x, y);
    return ImVec2(static_cast<float>(x), static_cast<float>(y));
}

static void ImGui_ImplSwizzle_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    vd->mWindow->setSize(static_cast<U32>(size.x), static_cast<U32>(size.y));
}

static void ImGui_ImplSwizzle_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    vd->mWindow->setTitle(title);
}

static bool ImGui_ImplSwizzle_GetWindowFocus(ImGuiViewport* viewport)
{
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    return vd->mWindow->hasFocus();
}

/* Function Definition */

bool ImGui_ImplSwizzle_Plf_Init(common::Resource<Window> window)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    ImGui_ImplSwizzle_PlfData* bd = IM_NEW(ImGui_ImplSwizzle_PlfData)();
    bd->mMainWindow = window;
    bd->mMainWindow->addEventListener(&bd->mInput);

    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_swizzle_platform";
    io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    // This bypasses the smart pointer, and is a potential use after free
    mainViewport->PlatformHandle = (void*)bd->mMainWindow.get();

    // Update monitors
    bd->mWantUpdateMonitors = true;
    ImGui_ImplSwizzle_UpdateMonitors();

    if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)
    {
        ImGui_ImplSwizzle_InitPlatformIfc();
    }

    return true;
}

void ImGui_ImplSwizzle_Plf_Shutdown()
{
    ImGui_ImplSwizzle_PlfData* bd = ImGui_ImplSwizzle_GetPlfBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSwizzle_ShutdownPlatformIfc();

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
    ImGui_ImplSwizzle_PlfViewportData* vd = (ImGui_ImplSwizzle_PlfViewportData*)viewport->PlatformUserData;
    return vd->mWindow;
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

void MainInputCallback::publishEvent(const swizzle::core::WindowEvent& evt)
{
    SWIZZLE_PROFILE_EVENT("MainInputCallback::publishEvent");

    auto evtType = evt.getEventType();
    switch (evtType)
    {
    case swizzle::core::WindowEventType::FocusEvent:
    {
        swizzle::core::WindowFocusEvent& e = (swizzle::core::WindowFocusEvent&)evt;
        ImGui_ImplSwizzle_FocusCallback(e.mWindow, e.mFocused);
        break;
    }
    case swizzle::core::WindowEventType::MouseEnterEvent:
    {
        swizzle::core::MouseEnterEvent& e = (swizzle::core::MouseEnterEvent&)evt;
        ImGui_ImplSwizzle_CursorEnterCallback(e.mWindow, e.mEnter);
        break;
    }
    case swizzle::core::WindowEventType::MouseMoveEvent:
    {
        swizzle::core::WindowMoveEvent& e = (swizzle::core::WindowMoveEvent&)evt;
        ImGui_ImplSwizzle_CursorPosCallback(e.mWindow, e.mXPos, e.mYPos);
        break;
    }
    case swizzle::core::WindowEventType::MouseScrollEvent:
    {
        swizzle::core::MouseScrollEvent& e = (swizzle::core::MouseScrollEvent&)evt;
        ImGui_ImplSwizzle_ScrollCallback(e.mWindow, e.mScrollX, e.mScrollY);
        break;
    }
    case swizzle::core::WindowEventType::KeyboardInputEvent:
    {
        swizzle::core::InputEvent& e = (swizzle::core::InputEvent&)evt;
        if (e.mFromKeyboard)
        {
            ImGui_ImplSwizzle_KeyEventCallback(e.mWindow, fromScanCode(e.mKey), e.mPressed);
        }
        else
        {
            ImGui_ImplSwizzle_MouseButtonCallback(e.mWindow, e.mKey - 1, e.mPressed);
        }
        break;
    }
    case swizzle::core::WindowEventType::CharacterTypeEvent:
    {
        swizzle::core::CharacterEvent& e = (swizzle::core::CharacterEvent&)evt;
        if (e.mCodePoint > 0 && e.mCodePoint < 0x10000)
        {
            ImGui_ImplSwizzle_CharCallback(e.mWindow, e.mCodePoint);
        }
        break;
    }
    default:
        break;
    }
}

void ViewportInputCallback::publishEvent(const swizzle::core::WindowEvent& evt)
{
    SWIZZLE_PROFILE_EVENT("ViewportInputCallback::publishEvent");

    auto evtType = evt.getEventType();
    switch (evtType)
    {
    case swizzle::core::WindowEventType::FocusEvent:
    {
        swizzle::core::WindowFocusEvent& e = (swizzle::core::WindowFocusEvent&)evt;
        ImGui_ImplSwizzle_FocusCallback(e.mWindow, e.mFocused);
        break;
    }
    case swizzle::core::WindowEventType::MouseEnterEvent:
    {
        swizzle::core::MouseEnterEvent& e = (swizzle::core::MouseEnterEvent&)evt;
        ImGui_ImplSwizzle_CursorEnterCallback(e.mWindow, e.mEnter);
        break;
    }
    case swizzle::core::WindowEventType::MouseMoveEvent:
    {
        swizzle::core::WindowMoveEvent& e = (swizzle::core::WindowMoveEvent&)evt;
        ImGui_ImplSwizzle_CursorPosCallback(e.mWindow, e.mXPos, e.mYPos);
        break;
    }
    case swizzle::core::WindowEventType::MouseScrollEvent:
    {
        swizzle::core::MouseScrollEvent& e = (swizzle::core::MouseScrollEvent&)evt;
        ImGui_ImplSwizzle_ScrollCallback(e.mWindow, e.mScrollX, e.mScrollY);
        break;
    }
    case swizzle::core::WindowEventType::KeyboardInputEvent:
    {
        swizzle::core::InputEvent& e = (swizzle::core::InputEvent&)evt;
        if (e.mFromKeyboard)
        {
            ImGui_ImplSwizzle_KeyEventCallback(e.mWindow, fromScanCode(e.mKey), e.mPressed);
        }
        else
        {
            ImGui_ImplSwizzle_MouseButtonCallback(e.mWindow, e.mKey - 1, e.mPressed);
        }
        break;
    }
        case swizzle::core::WindowEventType::CharacterTypeEvent:
    {
        swizzle::core::CharacterEvent& e = (swizzle::core::CharacterEvent&)evt;
        if (e.mCodePoint > 0 && e.mCodePoint < 0x10000)
        {
            ImGui_ImplSwizzle_CharCallback(e.mWindow, e.mCodePoint);
        }
        break;
    }
    case swizzle::core::WindowEventType::CloseEvent:
    {
        swizzle::core::WindowCloseEvent& e = (swizzle::core::WindowCloseEvent&)evt;
        if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(e.mWindow))
            viewport->PlatformRequestClose = true;
        break;
    }
    case swizzle::core::WindowEventType::MoveEvent:
    {
        swizzle::core::WindowCloseEvent& e = (swizzle::core::WindowCloseEvent&)evt;
        if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(e.mWindow))
            viewport->PlatformRequestMove = true;
        break;
    }
    case swizzle::core::WindowEventType::ResizeEvent:
    {
        swizzle::core::WindowCloseEvent& e = (swizzle::core::WindowCloseEvent&)evt;
        if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(e.mWindow))
            viewport->PlatformRequestResize = true;
        break;
    }
    case swizzle::core::WindowEventType::MouseMoveDeltaEvent:
    case swizzle::core::WindowEventType::GamepadAxisEvent:
    case swizzle::core::WindowEventType::GamepadButtonEvent:
    default:
        break;
    }
}

/* Class Private Function Definition */
