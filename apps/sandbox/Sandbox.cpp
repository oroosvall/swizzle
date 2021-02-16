
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

    Game game;

    game.initialize();
    game.run();

    sw::core::RemoveLogger(&logger);
}