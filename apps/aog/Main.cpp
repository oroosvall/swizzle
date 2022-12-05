#include <swizzle/Swizzle.hpp>
#include <utils/HighResolutionClock.hpp>

#include "Game.hpp"

#include <iostream>

class StdLogger : public swizzle::core::LogDevice
{
    // Inherited via LogDevice
    virtual void logMessage(const SwChar* messageType, const SwChar* message) override
    {
        printf("%s: %s\n", messageType, message);
    }

    StdLogger& operator=(const StdLogger& other)
    {
        // Guard self assignment
        if (this == &other)
            return *this;
    }

};

int main(int argv, char* argc[])
{
    UNUSED_ARG(argv);
    UNUSED_ARG(argc);

    setlocale(LC_CTYPE, "");

    StdLogger logger;

    sw::core::AddLogger(&logger);

    {
        Game game;
        game.initialize("Advent of graphics");
        game.run();
    }

    sw::core::RemoveLogger(&logger);
}
