#ifndef SWIZZLE_HPP
#define SWIZZLE_HPP

/* Includes */

#include <common/Types.hpp>
#include "swizzle/Api.hpp"

#include "swizzle/core/Logging.hpp"
#include "swizzle/core/Platform.hpp"
#include "swizzle/core/Window.hpp"

#include "swizzle/gfx/Color.hpp"
#include "swizzle/gfx/Context.hpp"
#include "swizzle/core/Input.hpp"

#include <utils/HighResolutionClock.hpp>

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

            mWindow = core::CreateWindow(1920, 1080, "Template");
            mWindow->show();

            input::SetInputSource(mWindow);

            gfx::GfxContextCreateInfo info = {};
            info.mDebugMemory = false;
            info.mDeviceIndex = 0u;

            mGfxContext = gfx::CreateContext(info);

            mSwapchain = mGfxContext->createSwapchain(mWindow, 3);
            mSwapchain->setVsync(gfx::VSyncTypes::vSyncAdaptive);

            userSetup();
        }

        void run()
        {
            utils::HighResolutionClock highRes;
            SwBool running = true;

            while (running)
            {
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

        core::Resource<gfx::GfxContext> mGfxContext;
        core::Resource<core::Window> mWindow;
        core::Resource<gfx::Swapchain> mSwapchain;

    };

} // namespace swizzle

namespace sw = swizzle;

#endif