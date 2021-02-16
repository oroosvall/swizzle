#ifndef SWIZZLE_HPP
#define SWIZZLE_HPP

/* Includes */

#include "swizzle/core/Types.hpp"
#include "swizzle/Api.hpp"

#include "swizzle/core/Logging.hpp"
#include "swizzle/core/Platform.hpp"
#include "swizzle/core/Window.hpp"

#include "swizzle/gfx/Gfx.hpp"
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
        void initialize()
        {
            SwInitialize();

            mWindow = core::CreateWindow(1920, 1080, "Template");
            mWindow->show();

            input::SetInputSource(mWindow);

            mSwapchain = gfx::CreateSwapchain(mWindow);
            mSwapchain->setVsync(gfx::VSyncTypes::vSyncAdaptive);

            userSetup();
        }

        void run()
        {
            utils::HighResolutionClock highRes;
            SwBool running = true;

            while (running)
            {
                float_t dt = highRes.secondsAsFloat(true);

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
            gfx::WaitIdle();
            userCleanup();

            mSwapchain.reset();
            mWindow.reset();

            SwCleanup();
        }

    protected:
        core::Resource<core::Window> mWindow;
        core::Resource<gfx::Swapchain> mSwapchain;

    };

} // namespace swizzle

namespace sw = swizzle;

#endif