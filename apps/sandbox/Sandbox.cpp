
#include <swizzle/Swizzle.hpp>
#include <utils/HighResolutionClock.hpp>

#include "Game.hpp"

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
    setlocale(LC_CTYPE, "");
    argc; argv;

    StdLogger logger;

    sw::core::AddLogger(&logger);
    sw::SwInitialize("Swizzle-Sandbox");

    printf("TempDir %s\n", sw::core::GetTempDirectory());
    printf("SaveGameDir %s\n", sw::core::GetSaveGameDirectory());
    printf("AppCache %s\n", sw::core::GetAppCacheDirectory());
    
    utils::HighResolutionClock highRes;

    Game game;

    while (true)
    {
        float_t dt = highRes.secondsAsFloat(true);
        if (!game.update(dt)) break;   
    }

    sw::gfx::WaitIdle();


    game.cleanup();

    sw::SwCleanup();
    sw::core::RemoveLogger(&logger);
}