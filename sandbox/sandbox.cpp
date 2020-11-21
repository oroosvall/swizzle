
#include <swizzle/Swizzle.hpp>
#include <utils/HighResolutionClock.hpp>

#include "game.hpp"

class StdLogger : public swizzle::core::LogDevice
{
    // Inherited via LogDevice
    virtual void logMessage(SwChar* messageType, SwChar* message) override
    {
        printf("%s: %s\n", messageType, message);
    }
};

int main(int argv, char* argc[])
{
    argc; argv;

    StdLogger logger;

    swizzle::core::AddLogger(&logger);
    swizzle::SwInitialize("Swizzle-Sandbox");

    printf("TempDir %s\n", swizzle::core::GetTempDirectory());
    printf("SaveGameDir %s\n", swizzle::core::GetSaveGameDirectory());
    printf("AppCache %s\n", swizzle::core::GetAppCacheDirectory());
    
    utils::HighResolutionClock highRes;

    Game* game = new Game();

    while (true)
    {
        float_t dt = highRes.secondsAsFloat(true);

        if (!game->update(dt)) break;   
    }

    sw::gfx::WaitIdle();

    delete game;

    swizzle::SwCleanup();
    swizzle::core::RemoveLogger(&logger);
}