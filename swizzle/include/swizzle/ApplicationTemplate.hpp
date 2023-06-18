
#pragma once

/* Includes */

#include <common/Common.hpp>
#include <swizzle/Swizzle.hpp>

#include "swizzle/core/Logging.hpp"
#include "swizzle/core/Platform.hpp"
#include "swizzle/core/Window.hpp"
#include "swizzle/profiler/Profiler.hpp"

#include "swizzle/gfx/Color.hpp"
#include "swizzle/gfx/Context.hpp"
#include "swizzle/core/Input.hpp"

#include <utils/HighResolutionClock.hpp>

/* Defines */

/* Typedefs */

/* Function Declaration */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle
{
    // TODO: move to "AppTemplate.hpp"
    class Application
    {
    public:
        void initialize(const SwChar* appName = "Template")
        {
            SwInitialize(appName);

            mWindow = core::CreateSwWindow(1920, 1080, appName);
            mWindow->show();

            input::SetInputSource(mWindow);

            gfx::GfxContextCreateInfo info = {};
            info.mDebugCallbacks = true;
            info.mDebugMemory = true;

            mGfxContext = gfx::CreateContext(info);

            U32 devCount = mGfxContext->getDeviceCount();
            U32 preferred = 0u;

            for (U32 i = 0u; i < devCount; ++i)
            {
                const SwChar* type = "Igpu";
                if (mGfxContext->isDiscreteGpu(i))
                {
                    preferred = i;
                    type = "Discrete";
                }
                LOG_INFO("%d: %s (%s)", i, mGfxContext->getDeviceName(i), type);
            }

            gfx::GfxContextInitializeInfo deviceInfo{};
            deviceInfo.mDeviceIndex = preferred;
            deviceInfo.mNumWorkerQueues = 1;
            deviceInfo.mNumTransferQueues = 1;

            mGfxDevice = mGfxContext->initializeDevice(deviceInfo);

            mSwapchain = mGfxDevice->createSwapchain(mWindow, 2);
            mSwapchain->setVsync(gfx::VSyncTypes::vSyncAdaptive);

            userSetup();
        }

        void run()
        {
            utils::HighResolutionClock highRes;
            SwBool running = true;

            while (running)
            {
                SWIZZLE_PROFILE_FRAME("MainThread");
                F32 dt = highRes.secondsAsFloat(true);

                swizzle::input::InputFrameReset();
                mWindow->pollEvents();

                running = userUpdate(dt);
            }

            cleanup();
        }

    protected:
        virtual void userSetup() = 0;
        virtual SwBool userUpdate(F32 dt) = 0;
        virtual void userCleanup() = 0;

    private:
        void cleanup()
        {
            mGfxDevice->waitIdle();
            userCleanup();

            mSwapchain.reset();
            mWindow.reset();

            mGfxDevice.reset();
            mGfxContext.reset();

            SwCleanup();
        }

    protected:

        common::Resource<gfx::GfxDevice> mGfxDevice = nullptr;
        common::Resource<gfx::GfxContext> mGfxContext = nullptr;
        common::Resource<core::SwWindow> mWindow = nullptr;
        common::Resource<gfx::Swapchain> mSwapchain = nullptr;

    };

} // namespace swizzle
