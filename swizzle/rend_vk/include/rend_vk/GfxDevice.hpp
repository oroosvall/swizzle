
#pragma once

/* Include files */

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace rvk
{
    enum class MemoryType
    {
        DeviceLocalHostVisible,
        DeviceLocal,
        Host
    };
} // namespace rvk

/* Class Declaration */

namespace rvk
{
    class GfxDevice
    {
    public:
        /// <summary>
        /// Waits until Gfx device has finished processing task and become idle
        /// </summary>
        virtual void waitIdle() const = 0;

        /// <summary>
        /// Get name for the device
        /// </summary>
        /// <returns>String contaiing the device name</returns>
        virtual std::string getDeviceName() const = 0;

    protected:
    };
} // namespace rvk

/* Function Declaration */
