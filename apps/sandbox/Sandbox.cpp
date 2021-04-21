
#include <swizzle/Swizzle.hpp>
#include <utils/HighResolutionClock.hpp>

#include "Game.hpp"

#include <swizzle/profile/Profiler.hpp>

class StdLogger : public swizzle::core::LogDevice
{
    // Inherited via LogDevice
    virtual void logMessage(const SwChar* messageType, const SwChar* message) override
    {
        printf("%s: %s\n", messageType, message);
    }
};

int main(int argv, char* argc[])
{
    setlocale(LC_CTYPE, "");
    (void)argc;
    (void)argv;

    StdLogger logger;

    sw::core::AddLogger(&logger);

    {
        Game game;

        game.initialize("Swizzle-sandbox");

        sw::profile::ProfileStart("ProfileTest.prof");

        game.run();

        sw::profile::ProfileEnd();
    }

    sw::core::RemoveLogger(&logger);
}