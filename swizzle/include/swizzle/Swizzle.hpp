#ifndef SWIZZLE_HPP
#define SWIZZLE_HPP

/* Includes */

#include <common/Common.hpp>
#include "swizzle/Api.hpp"

#include "swizzle/core/Logging.hpp"
#include "swizzle/core/Platform.hpp"
#include "swizzle/core/Window.hpp"

#include "swizzle/gfx/Color.hpp"
#include "swizzle/gfx/Context.hpp"
#include "swizzle/core/Input.hpp"

#include <utils/HighResolutionClock.hpp>

#include <optick/optick.h>

/* Defines */

/* Typedefs */

/* Function Declaration */

namespace swizzle
{
    SwBool SWIZZLE_API SwInitialize(const SwChar* appName = nullptr);
    SwBool SWIZZLE_API SwCleanup();
}

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

            for (U32 i = 0u; i < devCount; ++i) {
                const SwChar* type = "Igpu";
                if (mGfxContext->isDiscreteGpu(i)) {
                    preferred = i;
                    type = "Discrete";
                }
                LOG_INFO("%d: %s (%s)", i, mGfxContext->getDeviceName(i), type);
            }

            gfx::GfxContextInitializeInfo deviceInfo{};
            deviceInfo.mDeviceIndex = preferred;
            deviceInfo.mNumWorkerQueues = 1;
            deviceInfo.mNumTransferQueues = 1;

            mGfxContext->initializeDevice(deviceInfo);

            mSwapchain = mGfxContext->createSwapchain(mWindow, 2);
            mSwapchain->setVsync(gfx::VSyncTypes::vSyncAdaptive);

            userSetup();
        }

        void run()
        {
            utils::HighResolutionClock highRes;
            SwBool running = true;

            while (running)
            {
                OPTICK_FRAME("MainThread");
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
            mGfxContext->waitIdle();
            userCleanup();

            mSwapchain.reset();
            mWindow.reset();

            mGfxContext.reset();

            SwCleanup();
        }

    protected:

        common::Resource<gfx::GfxContext> mGfxContext;
        common::Resource<core::SwWindow> mWindow;
        common::Resource<gfx::Swapchain> mSwapchain;

    };

} // namespace swizzle

namespace sw = swizzle;

#endif
