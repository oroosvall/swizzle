
/* Include files */
#include "Sound.hpp"
#include <swizzle/core/Logging.hpp>

#ifdef PA_ENABLED
#include "portaudio.h"
#endif

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

#ifdef PA_ENABLED
static PaError sInitCode = paNoError;
#endif

/* Static Function Definition */

/* Function Definition */

namespace swizzle::sound
{
    SwBool SoundInitialize()
    {
#ifdef PA_ENABLED
        LOG_INFO("Initializing Sound!");
        sInitCode = Pa_Initialize();
        return sInitCode == paNoError;
#else
        LOG_INFO("Engine built without sound support");
        return true;
#endif
    }

    SwBool SoundCleanup()
    {
#ifdef PA_ENABLED
        if (sInitCode == paNoError)
        {
            sInitCode = Pa_Terminate();
        }
        return sInitCode == paNoError;
#else
        return true;
#endif
    }
} // namespace swizzle::sound

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
